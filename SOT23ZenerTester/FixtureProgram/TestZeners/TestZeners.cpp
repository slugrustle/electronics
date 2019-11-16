/**
 * TestZeners.cpp
 * 
 * TODO: proper file header
 * 
 * Written in 2019 by Ben Tesch.
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * The text of the CC0 Public Domain Dedication should be reproduced at the
 * end of this file. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <cstdio>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>
#define NOMINMAX
#include <Windows.h>
#include <WinBase.h>
#include "dln.h"
#include "dln_generic.h"
#include "dln_gpio.h"
#include "parse_config_file.h"
#include "SCPI_helpers.h"
#include "SCPI_transactions.h"
#include "BasicWorkbook.h"

/**
 * Constants
 */
const double MIN_RECORDABLE_CURRENT = 100.0e-9;

/**
 * Globals
 */
HDLN diolan_handle;
bool diolan_opened = false;
bool ctrl_c_happened = false;
bool GPIO_initialized = false;
HANDLE psu_com_port_handle;
bool psu_com_port_opened = false;
bool psu_modified = false;
HANDLE dmm_com_port_handle;
bool dmm_com_port_opened = false;
bool dmm_modified = false;
BasicWorkbook::Workbook workbook;
std::string workbook_filename;
bool workbook_started = false;

/**
 * Clean up function.
 * To be called before exiting normally or abnormally.
 * Should release all acquired resources.
 */
void exit_cleanup(void)
{
  DLN_RESULT dln_res;

  if (GPIO_initialized)
  {
    for (size_t jZener = 0u; jZener < NUM_ZENER_SOCKETS; jZener++)
    {
      dln_res = DlnGpioPinSetOutVal(diolan_handle, ZENER_RELAY_PIN.at(jZener), 0u);
      if (!DLN_SUCCEEDED(dln_res))
      {
        std::printf("Error: Could not set the level of GPIO pin %u.\n\n", ZENER_RELAY_PIN.at(jZener));
      }
    }
    for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
    {
      dln_res = DlnGpioPinSetOutVal(diolan_handle, RESISTOR_RELAY_PIN.at(jResistor), 0u);
      if (!DLN_SUCCEEDED(dln_res))
      {
        std::printf("Error: Could not set the level of GPIO pin %u.\n\n", RESISTOR_RELAY_PIN.at(jResistor));
      }
    }
  }

  if (diolan_opened)
  {
    dln_res = DlnCloseHandle(diolan_handle);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not properly close Diolan device.\n\n");
    }
  }

  if (psu_modified)
  {
    KnownStateStatus known_state_ret = set_known_PSU_state(psu_com_port_handle);
    if (known_state_ret != KnownStateStatus::ALL_OK)
    {
      std::printf("Error putting PSU into known state on exit.\n\n");
    }
  }

  if (psu_com_port_opened)
  {
    CloseHandle(psu_com_port_handle);
  }

  if (dmm_com_port_opened)
  {
    CloseHandle(dmm_com_port_handle);
  }

  if (workbook_started)
  {
    try
    {
      workbook.publish(workbook_filename);
    }
    catch (std::exception &e)
    {
      std::printf("Error creating workbook file %s.\n%s\n\n", workbook_filename.c_str(), e.what());
    }
  }
}

/**
 * A custom Ctrl + C handler to break the main loop.
 */
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
  if (fdwCtrlType == CTRL_C_EVENT || fdwCtrlType == CTRL_CLOSE_EVENT)
  {
    ctrl_c_happened = true;
    return true;
  }

  return false;
}

int main (int argc, char** argv)
{
  std::printf("Usage:\n\n");
  std::printf(".\\TestZeners.exe config_file.txt output_file.csv\n\n");

  if (argc != 3)
  {
    std::printf("This program requires exactly two input arguments.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
  {
    std::printf("Error: Could not set control handler.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::ifstream config_stream(argv[1]);
  if (!config_stream.is_open())
  {
    std::printf("Error: Could not open configuration file \"%s\" for reading.\n\n", argv[1]);
    exit_cleanup();
    return EXIT_FAILURE;
  }

  test_config_t test_config = parse_config_file(config_stream);
  config_stream.close();
  if (!test_config.read_ok)
  {
    std::printf("Error reading configuration file. Exiting.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  workbook_filename = argv[2];

  DLN_RESULT dln_res;

  uint32_t num_diolan_devices = 0u;
  dln_res = DlnGetDeviceCount(&num_diolan_devices);
  if (!DLN_SUCCEEDED(dln_res))
  {
    std::printf("Error: Could not get Diolan device count.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (num_diolan_devices == 0u)
  {
    std::printf("Error: No Diolan device is connected.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (num_diolan_devices > 1u)
  {
    if (!test_config.diolan_serial_present)
    {
      std::printf("Error: %u Diolan devices are connected, but no\n", num_diolan_devices);
      std::printf("  DIOLAN_SERIAL_NUMBER was specified in the configuration\n");
      std::printf("  file to pick which one to open.\n\n");
      std::printf("  Either connect only one Diolan device, or specify\n");
      std::printf("  which one to open by serial number.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }
    else
    {
      dln_res = DlnOpenDeviceBySn(test_config.diolan_serial_number, &diolan_handle);
      if (!DLN_SUCCEEDED(dln_res))
      {
        std::printf("Error: Could not open a Diolan device with serial number 0x%08X.\n\n", test_config.diolan_serial_number);
        exit_cleanup();
        return EXIT_FAILURE;
      }
    }
  }
  else
  {
    dln_res = DlnOpenDevice(0u, &diolan_handle);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not open the only connected Diolan device.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }
  }

  diolan_opened = true;
  std::printf("Opened diolan device");

  uint32_t diolan_serial = 0u;
  dln_res = DlnGetDeviceSn(diolan_handle, &diolan_serial);
  if (!DLN_SUCCEEDED(dln_res))
  {
    std::printf("\nError: Could not retrieve serial number from Diolan device.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf(" with serial 0x%08X.\n", diolan_serial);

  uint16_t pin_count = 0u;
  dln_res = DlnGpioGetPinCount(diolan_handle, &pin_count);
  if (!DLN_SUCCEEDED(dln_res))
  {
    std::printf("Error: Could not get Diolan device pin count\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (pin_count < test_config.num_gpio_required)
  {
    std::printf("Error: Connected Diolan device only has %u GPIO pins.\n", pin_count);
    std::printf("  This program requires at least %u GPIO pins.\n\n", test_config.num_gpio_required);
    exit_cleanup();
    return EXIT_FAILURE;
  }

  for (size_t jZener = 0u; jZener < NUM_ZENER_SOCKETS; jZener++)
  {
    uint16_t this_pin = ZENER_RELAY_PIN.at(jZener);

    dln_res = DlnGpioPinSetOutVal(diolan_handle, this_pin, 0u);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not set the level of GPIO pin %u.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }

    dln_res = DlnGpioPinSetDirection(diolan_handle, this_pin, 1u);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not set GPIO pin %u to be an output.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }

    dln_res = DlnGpioPinEnable(diolan_handle, this_pin);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not enable GPIO pin %u.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }
  }

  for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
  {
    uint16_t this_pin = RESISTOR_RELAY_PIN.at(jResistor);

    dln_res = DlnGpioPinSetOutVal(diolan_handle, this_pin, 0u);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not set the level of GPIO pin %u.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }

    dln_res = DlnGpioPinSetDirection(diolan_handle, this_pin, 1u);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not set GPIO pin %u to be an output.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }

    dln_res = DlnGpioPinEnable(diolan_handle, this_pin);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not enable GPIO pin %u.\n\n", this_pin);
      exit_cleanup();
      return EXIT_FAILURE;
    }
  }

  GPIO_initialized = true;

  dmm_com_port_handle = CreateFile(test_config.com_ports.at(static_cast<size_t>(SerialDevice::DMM)).c_str(), 
    GENERIC_READ | GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL);

  if (dmm_com_port_handle == INVALID_HANDLE_VALUE)
  {
    std::printf("Error opening COM port for DMM, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::DMM)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  dmm_com_port_opened = true;

  DCB dcbSerialParams = { 0 };
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  if (!GetCommState(dmm_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not retrieve parameters for DMM COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::DMM)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  dcbSerialParams.BaudRate          = static_cast<DWORD>(test_config.baud_rates.at(static_cast<size_t>(SerialDevice::DMM)));
  dcbSerialParams.fBinary           = true;
  dcbSerialParams.fParity           = (test_config.parities.at(static_cast<size_t>(SerialDevice::DMM)) != ParityType::NONE);
  dcbSerialParams.fOutxCtsFlow      = false;
  dcbSerialParams.fOutxDsrFlow      = false;
  dcbSerialParams.fDtrControl       = DTR_CONTROL_DISABLE;
  dcbSerialParams.fDsrSensitivity   = false;
  dcbSerialParams.fTXContinueOnXoff = true;
  dcbSerialParams.fOutX             = false;
  dcbSerialParams.fInX              = false;
  dcbSerialParams.fErrorChar        = false;
  dcbSerialParams.fNull             = false;
  dcbSerialParams.fRtsControl       = RTS_CONTROL_DISABLE;
  dcbSerialParams.fAbortOnError     = false;
  dcbSerialParams.wReserved         = 0;
  dcbSerialParams.ByteSize          = 8;
  dcbSerialParams.Parity            = static_cast<DWORD>(test_config.parities.at(static_cast<size_t>(SerialDevice::DMM)));
  dcbSerialParams.StopBits          = ONESTOPBIT;

  if (!SetCommState(dmm_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not set parameters for DMM COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::DMM)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  COMMTIMEOUTS timeouts = { 0 };
  timeouts.ReadIntervalTimeout         = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant    = 50;
  timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
  timeouts.WriteTotalTimeoutConstant   = 10;
  timeouts.WriteTotalTimeoutMultiplier = static_cast<DWORD>(std::ceil(1.1 * 10.0 * 1000.0 / static_cast<double>(test_config.baud_rates.at(static_cast<size_t>(SerialDevice::DMM)))));

  if (!SetCommTimeouts(dmm_com_port_handle, &timeouts))
  {
    std::printf("Error: Could not set timeouts for DMM COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::DMM)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }  

  std::printf("Opened COM port for DMM, %s.\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::DMM)).c_str());

  std::string dmm_id;
  Check_ID_status dmm_id_ret = check_DMM_ID(dmm_com_port_handle, dmm_id);
  if (dmm_id_ret == Check_ID_status::COM_ERROR)
  {
    std::printf("Error when trying to identify DMM over COM port.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (dmm_id_ret == Check_ID_status::ZERO_DATA)
  {
    std::printf("Error: No reply to SCPI *IDN? (identify device) query on DMM COM port.\n");
    std::printf("       Check your connections, baud rate settings, and configuration file.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (dmm_id_ret == Check_ID_status::ID_MISMATCH)
  {
    std::printf("Warning: Device on DMM COM port did not identify as a Rigol DM3068\n");
    std::printf("         multimeter. Other DMMs might not function properly with the\n");
    std::printf("         programmed command set. Continuing anyway.\n\n");
  }
  if (dmm_id_ret == Check_ID_status::ID_MATCH)
  {
    std::printf("DMM identified as a Rigol DM3068.\n");
  }

  std::printf("DMM ID string: %s\n", dmm_id.c_str());
  std::printf("Warning: You must manually set the Measurement Integration Time\n");
  std::printf("         to 10 NPLC as this setting is not accessible over SCPI.\n");
  std::printf("         Nearest options are 0.2 (not as accurate) and 100 (very\n");
  std::printf("         slow).\n\n");

  PSC_status psc_ret = set_PSC(dmm_com_port_handle);
  if (psc_ret == PSC_status::COM_ERROR)
  {
    std::printf("Error when trying to set DMM power-on status clear over COM port.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (psc_ret != PSC_status::PSC_ON)
  {
    std::printf("Warning: Unable to set DMM power-on status clear over COM port.\n");
    std::printf("         continuing anyway.\n\n");
  }

  KnownStateStatus known_state_ret = set_known_DMM_state(dmm_com_port_handle);
  if (known_state_ret != KnownStateStatus::ALL_OK)
  {
    std::printf("Error putting DMM into known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  OPC_status opc_ret = check_OPC(dmm_com_port_handle);
  if (opc_ret == OPC_status::OPC_FALSE)
  {
    std::printf("Error: DMM hung during attempt to establish known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  else if (opc_ret != OPC_status::OPC_TRUE)
  {
    std::printf("Error checking DMM command completion after setting known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("DMM set to known state.\n");

  psu_com_port_handle = CreateFile(test_config.com_ports.at(static_cast<size_t>(SerialDevice::PSU)).c_str(), 
                               GENERIC_READ | GENERIC_WRITE,
                               0, NULL, OPEN_EXISTING, 0, NULL);

  if (psu_com_port_handle == INVALID_HANDLE_VALUE)
  {
    std::printf("Error opening COM port for PSU, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::PSU)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  psu_com_port_opened = true;

  dcbSerialParams = { 0 };
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  if (!GetCommState(psu_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not retrieve parameters for PSU COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::PSU)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  dcbSerialParams.BaudRate          = static_cast<DWORD>(test_config.baud_rates.at(static_cast<size_t>(SerialDevice::PSU)));
  dcbSerialParams.fBinary           = true;
  dcbSerialParams.fParity           = (test_config.parities.at(static_cast<size_t>(SerialDevice::PSU)) != ParityType::NONE);
  dcbSerialParams.fOutxCtsFlow      = false;
  dcbSerialParams.fOutxDsrFlow      = false;
  dcbSerialParams.fDtrControl       = DTR_CONTROL_DISABLE;
  dcbSerialParams.fDsrSensitivity   = false;
  dcbSerialParams.fTXContinueOnXoff = true;
  dcbSerialParams.fOutX             = false;
  dcbSerialParams.fInX              = false;
  dcbSerialParams.fErrorChar        = false;
  dcbSerialParams.fNull             = false;
  dcbSerialParams.fRtsControl       = RTS_CONTROL_DISABLE;
  dcbSerialParams.fAbortOnError     = false;
  dcbSerialParams.wReserved         = 0;
  dcbSerialParams.ByteSize          = 8;
  dcbSerialParams.Parity            = static_cast<DWORD>(test_config.parities.at(static_cast<size_t>(SerialDevice::PSU)));
  dcbSerialParams.StopBits          = ONESTOPBIT;
  
  if (!SetCommState(psu_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not set parameters for PSU COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::PSU)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }
  
  timeouts = { 0 };
  timeouts.ReadIntervalTimeout         = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant    = 50;
  timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
  timeouts.WriteTotalTimeoutConstant   = 10;
  timeouts.WriteTotalTimeoutMultiplier = static_cast<DWORD>(std::ceil(1.1 * 10.0 * 1000.0 / static_cast<double>(test_config.baud_rates.at(static_cast<size_t>(SerialDevice::PSU)))));
  
  if (!SetCommTimeouts(psu_com_port_handle, &timeouts))
  {
    std::printf("Error: Could not set timeouts for PSU COM port, %s.\n\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::PSU)).c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }  
  
  std::printf("Opened COM port for PSU, %s.\n", test_config.com_port_printables.at(static_cast<size_t>(SerialDevice::PSU)).c_str());

  std::string psu_id;
  Check_ID_status psu_id_ret = check_PSU_ID(psu_com_port_handle, psu_id);
  if (psu_id_ret == Check_ID_status::COM_ERROR)
  {
    std::printf("Error when trying to identify PSU over COM port.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (psu_id_ret == Check_ID_status::ZERO_DATA)
  {
    std::printf("Error: No reply to SCPI *IDN? (identify device) query on PSU COM port.\n");
    std::printf("       Check your connections, baud rate settings, and configuration file.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (psu_id_ret == Check_ID_status::ID_MISMATCH)
  {
    std::printf("Warning: Device on PSU COM port did not identify as an Itech IT6100B series\n");
    std::printf("         power supply. Other PSUs might not function properly with the\n");
    std::printf("         programmed command set. Continuing anyway.\n\n");
  }
  if (psu_id_ret == Check_ID_status::ID_MATCH)
  {
    std::printf("PSU identified as Itech IT6100B series device.\n");
  }

  std::printf("PSU ID string: %s\n", psu_id.c_str());

  psc_ret = set_PSC(psu_com_port_handle);
  if (psc_ret == PSC_status::COM_ERROR)
  {
    std::printf("Error when trying to set PSU power-on status clear over COM port.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  if (psc_ret != PSC_status::PSC_ON)
  {
    std::printf("Warning: Unable to set PSU power-on status clear over COM port.\n");
    std::printf("         continuing anyway.\n\n");
  }

  known_state_ret = set_known_PSU_state(psu_com_port_handle);
  if (known_state_ret != KnownStateStatus::ALL_OK)
  {
    std::printf("Error putting PSU into known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  opc_ret = check_OPC(psu_com_port_handle);
  if (opc_ret == OPC_status::OPC_FALSE)
  {
    std::printf("Error: PSU hung during attempt to establish known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  else if (opc_ret != OPC_status::OPC_TRUE)
  {
    std::printf("Error checking PSU command completion after setting known state.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("PSU set to known state.\n");

  PSU_limits_t PSU_limits;
  GetDataStatus get_ret = get_PSU_limits(psu_com_port_handle, PSU_limits);
  if (get_ret != GetDataStatus::ALL_OK)
  {
    std::printf("Error while retrieving PSU limits.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  double volt_protect_level = std::min(std::round(PSU_limits.voltage_out_max) + 1.0, PSU_limits.voltage_protect_max);

  /* Set PSU to remote mode so we can start changing settings. */
  SCPIstatus SCPIret = SCPIwrite(psu_com_port_handle, "SYST:REM");
  if (SCPIret != SCPIstatus::ALL_OK)
  {
    std::printf("Error while setting PSU to remote mode.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  psu_modified = true;

  SetDataStatus set_ret = set_PSU_protection(psu_com_port_handle, volt_protect_level);
  if (set_ret != SetDataStatus::ALL_OK)
  {
    std::printf("Error while setting PSU voltage protection to %.3fV and turning it on.\n\n", volt_protect_level);
    exit_cleanup();
    return EXIT_FAILURE;
  }

  /* Determine max current based on resistor power. */
  double max_current = 0.0;
  for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
  {
    max_current = std::max(max_current, std::sqrt(test_config.max_resistor_powers.at(jResistor) / NOMINAL_RESISTANCES.at(jResistor)));
  }
  max_current *= 1.1;
  max_current = 0.001 * std::round(1000.0 * max_current);

  set_ret = set_PSU_current(psu_com_port_handle, max_current);
  if (set_ret != SetDataStatus::ALL_OK)
  {
    std::printf("Error while setting PSU current to %.3fA.\n\n", max_current);
    exit_cleanup();
    return EXIT_FAILURE;
  }

  set_ret = set_PSU_sense(psu_com_port_handle, true);
  if (set_ret != SetDataStatus::ALL_OK)
  {
    std::printf("Error while setting PSU to remote sense mode.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  /**
   * The main loop.
   *
   * Iterate over installed Zeners and get the Vz/Iz curve for each.
   */
  for (std::set<ZenerSocket>::iterator zenitr = test_config.zeners_installed.begin();
       zenitr != test_config.zeners_installed.end();
       zenitr++)
  {
    if (ctrl_c_happened)
    {
      std::printf("Detected Ctrl + C; Exiting.\n\n");
      exit_cleanup();
      return EXIT_SUCCESS;
    }

    ZenerSocket this_zener = *zenitr;

    BasicWorkbook::Sheet &this_sheet = workbook.addSheet(ZENER_SOCKET_NAMES.at(static_cast<size_t>(this_zener)));
    uint32_t sheet_row = 1u;
    this_sheet.add_string_cell(sheet_row, 1u, "PSU Voltage Setting (V)");
    this_sheet.set_column_width(1u, 20.005);
    this_sheet.add_string_cell(sheet_row, 2u, "Measured PSU Voltage (V)");
    this_sheet.set_column_width(2u, 22.625);
    this_sheet.add_string_cell(sheet_row, 3u, "Measured Zener Voltage (V)");
    this_sheet.set_column_width(3u, 24.125);
    this_sheet.add_string_cell(sheet_row, 4u, "Nominal Test Resistance (Ohms)");
    this_sheet.set_column_width(4u, 28.005);
    this_sheet.add_string_cell(sheet_row, 5u, "Calculated Zener Current (A)");
    this_sheet.set_column_width(5u, 24.375);
    sheet_row++;
    workbook_started = true;

    std::printf("Testing Zener Socket %s\n", ZENER_SOCKET_NAMES.at(static_cast<size_t>(this_zener)).c_str());
    std::printf("PSU Voltage Setting (V),Measured PSU Voltage (V),Measured Zener Voltage (V),Nominal Test Resistance (Ohms),Calculated Zener Current (A)\n");

    /* Make sure the PSU output is off. */
    set_ret = set_PSU_output(psu_com_port_handle, false);
    if (set_ret != SetDataStatus::ALL_OK)
    {
      std::printf("Error while turning PSU output off.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    /* Open all Zener relays. */
    for (size_t jZener = 0u; jZener < NUM_ZENER_SOCKETS; jZener++)
    {
      dln_res = DlnGpioPinSetOutVal(diolan_handle, ZENER_RELAY_PIN.at(jZener), 0u);
      if (!DLN_SUCCEEDED(dln_res))
      {
        std::printf("Error: Could not set the level of GPIO pin %u.\n\n", ZENER_RELAY_PIN.at(jZener));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    /* Close the relay for the one Zener under test. */
    dln_res = DlnGpioPinSetOutVal(diolan_handle, ZENER_RELAY_PIN.at(static_cast<size_t>(this_zener)), 1u);
    if (!DLN_SUCCEEDED(dln_res))
    {
      std::printf("Error: Could not set the level of GPIO pin %u.\n\n", ZENER_RELAY_PIN.at(static_cast<size_t>(this_zener)));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    /**
     * Iterate over fixture resistors from highest to lowest value
     * in order to test Zener currents in increasing fashion.
     */
    double prev_itr_max_zener_current = 0.0;
    double this_itr_max_zener_current = 0.0;
    double prev_itr_zener_voltage_at_max_current = 0.0;
    double this_itr_zener_voltage_at_max_current = 0.0;
    size_t jResistor = NUM_FIXTURE_RESISTORS;
    do
    {
      if (ctrl_c_happened)
      {
        std::printf("Detected Ctrl + C; Exiting.\n\n");
        exit_cleanup();
        return EXIT_SUCCESS;
      }

      jResistor--;
      if (this_itr_max_zener_current >= prev_itr_max_zener_current)
      {
        prev_itr_max_zener_current = this_itr_max_zener_current;
        prev_itr_zener_voltage_at_max_current = this_itr_zener_voltage_at_max_current;
      }
      this_itr_max_zener_current = 0.0;
      this_itr_zener_voltage_at_max_current = 0.0;

      /* Make sure the PSU output is off. */
      set_ret = set_PSU_output(psu_com_port_handle, false);
      if (set_ret != SetDataStatus::ALL_OK)
      {
        std::printf("Error while turning PSU output off.\n\n");
        exit_cleanup();
        return EXIT_FAILURE;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      /* Open all resistor relays. */
      for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
      {
        dln_res = DlnGpioPinSetOutVal(diolan_handle, RESISTOR_RELAY_PIN.at(jResistor), 0u);
        if (!DLN_SUCCEEDED(dln_res))
        {
          std::printf("Error: Could not set the level of GPIO pin %u.\n\n", RESISTOR_RELAY_PIN.at(jResistor));
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      /* Close the relay for the indicated fixture resistor. */
      dln_res = DlnGpioPinSetOutVal(diolan_handle, RESISTOR_RELAY_PIN.at(jResistor), 1u);
      if (!DLN_SUCCEEDED(dln_res))
      {
        std::printf("Error: Could not set the level of GPIO pin %u.\n\n", RESISTOR_RELAY_PIN.at(jResistor));
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      
      /**
       * Jump to highest current level tested by previous fixture
       * resistor right away if it skips some voltage steps and
       * won't overload the fixture resistor or exceed the max
       * zener test current.
       */
      bool revert_to_min_voltage = true;
      double test_voltage = test_config.voltage_steps.at(static_cast<size_t>(this_zener))
                            * std::floor((prev_itr_zener_voltage_at_max_current + NOMINAL_RESISTANCES.at(jResistor) * prev_itr_max_zener_current) 
                                         / test_config.voltage_steps.at(static_cast<size_t>(this_zener)))
                            - test_config.voltage_steps.at(static_cast<size_t>(this_zener));
      double estimated_resistor_power = std::pow(test_voltage - prev_itr_zener_voltage_at_max_current, 2.0) / NOMINAL_RESISTANCES.at(jResistor);
      double estimated_zener_current = (test_voltage - prev_itr_zener_voltage_at_max_current) / NOMINAL_RESISTANCES.at(jResistor);
      for (uint8_t jAttempt = 0u; jAttempt < 5u; jAttempt++)
      {
        if (test_voltage > 0.0 && 
            test_voltage > PSU_limits.voltage_out_min && 
            estimated_resistor_power < 0.95 * test_config.max_resistor_powers.at(jResistor) &&
            estimated_zener_current < 0.95 * test_config.max_zener_currents.at(static_cast<size_t>(this_zener)))
        {
          revert_to_min_voltage = false;
          break;
        }
        else
        {
          test_voltage -= test_config.voltage_steps.at(static_cast<size_t>(this_zener));
          estimated_resistor_power = std::pow(test_voltage - prev_itr_zener_voltage_at_max_current, 2.0) / NOMINAL_RESISTANCES.at(jResistor);
          estimated_zener_current = (test_voltage - prev_itr_zener_voltage_at_max_current) / NOMINAL_RESISTANCES.at(jResistor);
        }
      }

      if (revert_to_min_voltage)
      {
        /* Check assumed fixture resistor power. */
        test_voltage = PSU_limits.voltage_out_min;
        double worst_case_resistor_power = test_voltage * test_voltage / NOMINAL_RESISTANCES.at(jResistor);
        if (worst_case_resistor_power > test_config.max_resistor_powers.at(jResistor))
        {
          std::printf("Warning: Minimum voltage level %.3fV could exceed configured\n", test_voltage);
          std::printf("         maximum resistor power %.3fW for %.1f Ohm resistor.\n", 
            test_config.max_resistor_powers.at(jResistor), NOMINAL_RESISTANCES.at(jResistor));
          std::printf("         Skipping this resistor.\n\n");
          continue;
        }
      }

      /* Set initial PSU voltage and turn output on. */
      set_ret = set_PSU_voltage(psu_com_port_handle, test_voltage);
      if (set_ret != SetDataStatus::ALL_OK)
      {
        std::printf("Error while setting PSU voltage to %.3fV.\n\n", test_voltage);
        exit_cleanup();
        return EXIT_FAILURE;
      }

      set_ret = set_PSU_output(psu_com_port_handle, true);
      if (set_ret != SetDataStatus::ALL_OK)
      {
        std::printf("Error while turning PSU output on.\n\n");
        exit_cleanup();
        return EXIT_FAILURE;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      bool break_due_to_zener_current = false;
      bool break_due_to_zener_power = false;

      for (/* nothing */; 
           test_voltage < PSU_limits.voltage_out_max;
           test_voltage += test_config.voltage_steps.at(static_cast<size_t>(this_zener)))
      {
        if (ctrl_c_happened)
        {
          std::printf("Detected Ctrl + C; Exiting.\n\n");
          exit_cleanup();
          return EXIT_SUCCESS;
        }

        set_ret = set_PSU_voltage(psu_com_port_handle, test_voltage);
        if (set_ret != SetDataStatus::ALL_OK)
        {
          std::printf("Error while setting PSU voltage to %.3fV.\n\n", test_voltage);
          exit_cleanup();
          return EXIT_FAILURE;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        /**
         * Take & record measurements.
         * .csv columns are
         * PSU Voltage Setting (V),Measured Zener Voltage (V),Nominal Test Resistance (Ohms),Calculated Zener Current (A)
         */
        double meas_psu_voltage = 0.0;
        double dvm_zener_voltage = 0.0;
        double calc_zener_current = 0.0;

        get_ret = meas_DMM_DCV(dmm_com_port_handle, dvm_zener_voltage);
        if (get_ret != GetDataStatus::ALL_OK && get_ret != GetDataStatus::COM_ERROR)
        {
          /* try a second time if ZERO_DATA or UNRECOGNIZED_RESPONSE */
          get_ret = meas_DMM_DCV(dmm_com_port_handle, dvm_zener_voltage);
        }
        if (get_ret != GetDataStatus::ALL_OK)
        {
          std::printf("Error retrieving DMM DC Voltage measurement.\n\n");
          exit_cleanup();
          return EXIT_FAILURE;
        }

        get_ret = meas_PSU_voltage(psu_com_port_handle, meas_psu_voltage);
        if (get_ret != GetDataStatus::ALL_OK)
        {
          std::printf("Error retrieving PSU Voltage measurement.\n\n");
          exit_cleanup();
          return EXIT_FAILURE;
        }

        calc_zener_current = (meas_psu_voltage - dvm_zener_voltage) / NOMINAL_RESISTANCES.at(jResistor);
        if (calc_zener_current >= this_itr_max_zener_current)
        {
          this_itr_max_zener_current = calc_zener_current;
          this_itr_zener_voltage_at_max_current = dvm_zener_voltage;
        }

        if (calc_zener_current >= MIN_RECORDABLE_CURRENT && calc_zener_current >= 1.01 * prev_itr_max_zener_current)
        {
          this_sheet.add_number_cell(sheet_row, 1u, test_voltage, BasicWorkbook::NumberFormat::FIX3);
          this_sheet.add_number_cell(sheet_row, 2u, meas_psu_voltage, BasicWorkbook::NumberFormat::FIX6);
          this_sheet.add_number_cell(sheet_row, 3u, dvm_zener_voltage, BasicWorkbook::NumberFormat::FIX6);
          this_sheet.add_number_cell(sheet_row, 4u, NOMINAL_RESISTANCES.at(jResistor), BasicWorkbook::NumberFormat::FIX1);
          std::string zener_current_formula = "(" + BasicWorkbook::integerref_to_mixedref(sheet_row, 2u) +
            "-" + BasicWorkbook::integerref_to_mixedref(sheet_row, 3u) + ") / " +
            BasicWorkbook::integerref_to_mixedref(sheet_row, 4u);
          this_sheet.add_formula_cell(sheet_row, 5u, zener_current_formula, BasicWorkbook::NumberFormat::SCI5);
          sheet_row++;
        }

        std::printf("%.6e,%.6e,%.6e,%.1f,%.6e\n", test_voltage, meas_psu_voltage, dvm_zener_voltage, NOMINAL_RESISTANCES.at(jResistor), calc_zener_current);

        double next_voltage_level = test_voltage + test_config.voltage_steps.at(static_cast<size_t>(this_zener));
        if (dvm_zener_voltage > 0.0 && next_voltage_level > dvm_zener_voltage)
        {
          /* Check assumed fixture resistor power at next voltage level */
          double next_res_power = std::pow(next_voltage_level - dvm_zener_voltage, 2.0) / NOMINAL_RESISTANCES.at(jResistor);
          if (next_res_power > test_config.max_resistor_powers.at(jResistor)) break;

          /* Check assumed Zener current at next voltage level */
          double next_zener_current = (next_voltage_level - dvm_zener_voltage) / NOMINAL_RESISTANCES.at(jResistor);
          if (next_zener_current > test_config.max_zener_currents.at(static_cast<size_t>(this_zener)))
          {
            break_due_to_zener_current = true;
            break;
          }

          /* Check assumed Zener diode power at next voltage level */
          double next_zener_power = dvm_zener_voltage * (next_voltage_level - dvm_zener_voltage) / NOMINAL_RESISTANCES.at(jResistor);
          if (next_zener_power > test_config.max_zener_powers.at(static_cast<size_t>(this_zener)))
          {
            break_due_to_zener_power = true;
            break;
          }
        }
      }

      /**
       * Testing at a lower resistance will not help with either a
       * Zener current limit or a Zener power limit.
       */
      if (break_due_to_zener_current || break_due_to_zener_power) break;

    } while (jResistor > 0u);
  }

  try
  {
    workbook_started = false;
    workbook.publish(workbook_filename);
  }
  catch (std::exception &e)
  {
    std::printf("Error creating workbook file %s.\n%s\n\n", workbook_filename.c_str(), e.what());
    exit_cleanup();
    return EXIT_FAILURE;
  }
  
  exit_cleanup();
  return EXIT_SUCCESS;
}

/*
Creative Commons Legal Code

CC0 1.0 Universal

CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
HEREUNDER.

Statement of Purpose

The laws of most jurisdictions throughout the world automatically confer
exclusive Copyright and Related Rights (defined below) upon the creator
and subsequent owner(s) (each and all, an "owner") of an original work of
authorship and/or a database (each, a "Work").

Certain owners wish to permanently relinquish those rights to a Work for
the purpose of contributing to a commons of creative, cultural and
scientific works ("Commons") that the public can reliably and without fear
of later claims of infringement build upon, modify, incorporate in other
works, reuse and redistribute as freely as possible in any form whatsoever
and for any purposes, including without limitation commercial purposes.
These owners may contribute to the Commons to promote the ideal of a free
culture and the further production of creative, cultural and scientific
works, or to gain reputation or greater distribution for their Work in
part through the use and efforts of others.

For these and/or other purposes and motivations, and without any
expectation of additional consideration or compensation, the person
associating CC0 with a Work (the "Affirmer"), to the extent that he or she
is an owner of Copyright and Related Rights in the Work, voluntarily
elects to apply CC0 to the Work and publicly distribute the Work under its
terms, with knowledge of his or her Copyright and Related Rights in the
Work and the meaning and intended legal effect of CC0 on those rights.

1. Copyright and Related Rights. A Work made available under CC0 may be
protected by copyright and related or neighboring rights ("Copyright and
Related Rights"). Copyright and Related Rights include, but are not
limited to, the following:

i. the right to reproduce, adapt, distribute, perform, display,
communicate, and translate a Work;
ii. moral rights retained by the original author(s) and/or performer(s);
iii. publicity and privacy rights pertaining to a person's image or
likeness depicted in a Work;
iv. rights protecting against unfair competition in regards to a Work,
subject to the limitations in paragraph 4(a), below;
v. rights protecting the extraction, dissemination, use and reuse of data
in a Work;
vi. database rights (such as those arising under Directive 96/9/EC of the
European Parliament and of the Council of 11 March 1996 on the legal
protection of databases, and under any national implementation
thereof, including any amended or successor version of such
directive); and
vii. other similar, equivalent or corresponding rights throughout the
world based on applicable law or treaty, and any national
implementations thereof.

2. Waiver. To the greatest extent permitted by, but not in contravention
of, applicable law, Affirmer hereby overtly, fully, permanently,
irrevocably and unconditionally waives, abandons, and surrenders all of
Affirmer's Copyright and Related Rights and associated claims and causes
of action, whether now known or unknown (including existing as well as
future claims and causes of action), in the Work (i) in all territories
worldwide, (ii) for the maximum duration provided by applicable law or
treaty (including future time extensions), (iii) in any current or future
medium and for any number of copies, and (iv) for any purpose whatsoever,
including without limitation commercial, advertising or promotional
purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
member of the public at large and to the detriment of Affirmer's heirs and
successors, fully intending that such Waiver shall not be subject to
revocation, rescission, cancellation, termination, or any other legal or
equitable action to disrupt the quiet enjoyment of the Work by the public
as contemplated by Affirmer's express Statement of Purpose.

3. Public License Fallback. Should any part of the Waiver for any reason
be judged legally invalid or ineffective under applicable law, then the
Waiver shall be preserved to the maximum extent permitted taking into
account Affirmer's express Statement of Purpose. In addition, to the
extent the Waiver is so judged Affirmer hereby grants to each affected
person a royalty-free, non transferable, non sublicensable, non exclusive,
irrevocable and unconditional license to exercise Affirmer's Copyright and
Related Rights in the Work (i) in all territories worldwide, (ii) for the
maximum duration provided by applicable law or treaty (including future
time extensions), (iii) in any current or future medium and for any number
of copies, and (iv) for any purpose whatsoever, including without
limitation commercial, advertising or promotional purposes (the
"License"). The License shall be deemed effective as of the date CC0 was
applied by Affirmer to the Work. Should any part of the License for any
reason be judged legally invalid or ineffective under applicable law, such
partial invalidity or ineffectiveness shall not invalidate the remainder
of the License, and in such case Affirmer hereby affirms that he or she
will not (i) exercise any of his or her remaining Copyright and Related
Rights in the Work or (ii) assert any associated claims and causes of
action with respect to the Work, in either case contrary to Affirmer's
express Statement of Purpose.

4. Limitations and Disclaimers.

a. No trademark or patent rights held by Affirmer are waived, abandoned,
surrendered, licensed or otherwise affected by this document.
b. Affirmer offers the Work as-is and makes no representations or
warranties of any kind concerning the Work, express, implied,
statutory or otherwise, including without limitation warranties of
title, merchantability, fitness for a particular purpose, non
infringement, or the absence of latent or other defects, accuracy, or
the present or absence of errors, whether or not discoverable, all to
the greatest extent permissible under applicable law.
c. Affirmer disclaims responsibility for clearing rights of other persons
that may apply to the Work or any use thereof, including without
limitation any person's Copyright and Related Rights in the Work.
Further, Affirmer disclaims responsibility for obtaining any necessary
consents, permissions or other rights required for any use of the
Work.
d. Affirmer understands and acknowledges that Creative Commons is not a
party to this document and has no duty or obligation with respect to
this CC0 or use of the Work.
*/
