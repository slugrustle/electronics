/**
 * HH509R_Logger.cpp
 * 
 * This program logs temperature data from an Omega HH509R thermocouple reader.
 * It both prints the present readings to screen and saves all logged data
 * in an output .xlsx workbook file.
 * 
 * This program should be run from PowerShell or the command line as follows:
 * .\HH509R_Logger.exe COM_PORT output_file.xlsx
 *
 * Where COM_PORT is COM1, COM2, COM3, etc.
 *
 * The thermocouple reader should be powered on before HH509R_Logger.exe is run.
 *
 * The program ceases data collection and saves outputs under four conditions:
 *   1. The user keys Ctrl+C in the command line / PowerShell window.
 *   2. No data of any kind is received from the thermocouple reader for 60 seconds.
 *   3. No valid data is received from the thermocouple reader for 300 seconds.
 *   4. The amount of collected data has reached the row limit of a .xlsx workbook file.
 *
 * Written in 2020 by Ben Tesch.
 * Originally distributed at https://github.com/slugrustle/electronics
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * The text of the CC0 Public Domain Dedication should be reproduced at the
 * end of this file. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <cinttypes>
#include <cstdio>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <deque>
#include <limits>
#include <algorithm>
#include <fstream>
#define NOMINMAX
#include <Windows.h>
#include <WinBase.h>
#include "parser_helpers.h"
#include "BasicWorkbook.h"

/*******************************************************
 ***                    Types                        ***
 *******************************************************/

/**
 * Stores the unparsed data line (minus \r\n terminator)
 * received from the thermocouple reader as well as the timestamp
 * (based on the computer's steady_clock) at which it was
 * received.
 */
typedef struct
{
  std::string sample_string;
  double sample_time_s;
} sample_and_timestamp_t;

/**
 * Stores the possible states of the data line character
 * that indicates thermocouple type.
 * The same type applies to both thermocouples.
 */
enum class TC_type_t : uint8_t
{
  K_type,
  J_type,
  T_type,
  E_type,
  S_type
};

/**
 * Stores the possible states of the data line character
 * that indicates the status of Record mode.
 */
enum class Record_mode_t : uint8_t
{
  Record,
  Max,
  Min,
  Average,
  Range,
  Normal
};

/**
 * Stores the possible states of the data line character
 * that indicates when Rel mode or Hold mode is active.
 */
enum class Rel_Hold_mode_t : uint8_t
{
  Rel,
  Hold,
  Normal
};

/**
 * Stores all the information contained in a single data line
 * from the thermocouple reader.
 * Since the thermocouple reader timestamp is inconsistent,
 * a timestamp from the computer's steady_clock is utilized
 * instead.
 */
typedef struct
{
  double T1_degC;
  double T2_degC;
  TC_type_t tc_type;
  Record_mode_t record_mode;
  Rel_Hold_mode_t rel_hold_mode;
  bool limits_mode;
  bool battery_low;
  double timestamp;
} parsed_sample_t;

/**
 * Enum enumerating all possible commands that may be sent
 * to the thermocouple reader.
 */
enum class TC_command_t : uint8_t
{
  Activate_data_transmission,
  Stop_data_transmission,
  Celsius_Fahrenheit_key,
  Backlight_key,
  HOLD_key,
  TYPE_key,
  MIN_MAX_key,
  Exit_record_mode,
  T1_key,
  T2_key,
  T1_minus_T2_key,
  REL_key,
  Hi_Lo_LIMITS_key,
  Turn_off_power
};

/*******************************************************
 ***                    Globals                      ***
 *******************************************************/

/**
 * ctrl_c_happened is set to true in the Ctrl+C handler
 * and acted upon in the measurement loop.
 */
static bool ctrl_c_happened = false;

/**
 * The following manage the COM port specified by the user,
 * to which a powered on HH509R thermocouple reader is hopefully
 * connected.
 */
static HANDLE tc_reader_com_port_handle;
static bool tc_reader_com_port_opened = false;
static std::string tc_reader_com_port;
static std::string tc_reader_com_port_printable;

/**
 * tc_reader_modified is set to true after we ask the thermocouple
 * reader to start sending data so we can remember to ask it to
 * stop sending data in the exit / cleanup routine.
 */
static bool tc_reader_modified = false;

/**
 * Buffer for reading data from the COM port.
 * Hopefully it's HH509R thermocouple reader data...
 */
static const size_t read_buf_size = 320u;
static char read_buffer[read_buf_size];

static std::ofstream log_file_handle;
static std::ofstream log_file2_handle;

/**
 * Human readable calendar date and time of test start.
 * Purely for placing in header of output file.
 */
static std::string test_start_date_string;
static std::string test_start_time_string;

/**
 * The thermocouple reader sends each data line 3 times
 * in a row. This program uses that fact to avoid storing
 * corrupt data.
 */
static const uint8_t transmissions_per_sample = 3u;

/**
 * Vector holding all validated measurement data during
 * collection. This is only written to the output file
 * upon exit.
 */
static std::vector<parsed_sample_t> data_vec;

/**
 * Filename for output file.
 */
static std::string workbook_filename;

/**
 * Row index of first row where temperature data is recorded
 * in output .xlsx workbook file.
 *
 * This is one-indexed. Blame the office open XML standard.
 */
static const uint32_t first_data_row = 4u;

/**
 * Delays and timeouts pertaining to reading data from
 * and writing data to the thermocouple reader.
 */
static const uint32_t poll_min_wait_ms = 10u;
static const uint32_t intersend_wait_ms = 600u;
static const uint32_t mode_change_retry_ms = 1350u;
static const uint32_t restart_retry_s = 2u;
static const uint32_t restart_wait_s = 4u;
static const uint32_t no_data_timeout_s = 60u;
static const uint32_t no_valid_data_timeout_s = 300u;

/**
 * Minimum time difference for which to print the range of
 * the T1-T2 reading.
 */
static const double min_T1_T2_lookback_s = 300.0;

/**
 * Lengths and character indices related to the
 * thermocouple reader data line.
 */
static const size_t valid_data_line_length = 30u;
static const size_t line_terminator_length = 2u;
static const size_t T1_temp_sign_index = 0u;
static const size_t T1_temp_MSD_index = 1u;
static const size_t T1_temp_LSD_index = 6u;
static const size_t TC_type_char_index = 7u;
static const size_t T2_temp_sign_index = 8u;
static const size_t T2_temp_MSD_index = 9u;
static const size_t T2_temp_LSD_index = 14u;
static const size_t Unused_char_1_index = 15u;
static const size_t Hours_MSD_index = 16u;
static const size_t Hours_LSD_index = 17u;
static const size_t Minutes_MSD_index = 18u;
static const size_t Minutes_LSD_index = 19u;
static const size_t Seconds_MSD_index = 20u;
static const size_t Seconds_LSD_index = 21u;
static const size_t Record_mode_char_index = 22u;
static const size_t Rel_hold_mode_char_index = 23u;
static const size_t Unused_char_2_index = 24u;
static const size_t Limits_mode_char_index = 25u;
static const size_t Hi_limit_char_index = 26u;
static const size_t Lo_limit_char_index = 27u;
static const size_t Unused_char_3_index = 28u;
static const size_t Battery_status_char_index = 29u;

/**
 * Converts a TC_type_t enum value to the corresponding
 * printable character value.
 */
char TC_type_enum_to_char(TC_type_t tc_type)
{
  switch(tc_type)
  {
  case TC_type_t::K_type:
    return 'K';
  case TC_type_t::J_type:
    return 'J';
  case TC_type_t::T_type:
    return 'T';
  case TC_type_t::E_type:
    return 'E';
  case TC_type_t::S_type:
    return 'S';
  default:
    return 'K';
  }
}

/**
 * This function converts the sign/magnitude six-digit hexadecimal
 * integer millidegree Celsius temperature readings sent by the
 * thermocouple reader into double floating point degrees Celsius.
 */
double signmag_hexint_mdegC_to_degC(const std::string &input, size_t sign_index, size_t MSD_index)
{
  static const double sixteen_pow_1 = 16.0;
  static const double sixteen_pow_2 = sixteen_pow_1 * 16.0;
  static const double sixteen_pow_3 = sixteen_pow_2 * 16.0;
  static const double sixteen_pow_4 = sixteen_pow_3 * 16.0;
  static const double sixteen_pow_5 = sixteen_pow_4 * 16.0;
  static const double raw_temp_to_degC_scalar = 0.001;

  double output_degC = raw_temp_to_degC_scalar * (
    static_cast<double>(parse_hex_char(input.at(MSD_index + 5u))) + 
    sixteen_pow_1 * static_cast<double>(parse_hex_char(input.at(MSD_index + 4u))) + 
    sixteen_pow_2 * static_cast<double>(parse_hex_char(input.at(MSD_index + 3u))) +
    sixteen_pow_3 * static_cast<double>(parse_hex_char(input.at(MSD_index + 2u))) +
    sixteen_pow_4 * static_cast<double>(parse_hex_char(input.at(MSD_index + 1u))) +
    sixteen_pow_5 * static_cast<double>(parse_hex_char(input.at(MSD_index))) );
  
  if (input.at(sign_index) == '-') output_degC *= -1.0;

  return output_degC;
}

/**
 * Helper function for sending single letter plus terminator
 * commands to thermocouple reader over COM port.
 *
 * Returns false on failure (clean up & exit main) and true
 * on success.
 */
bool send_command(TC_command_t command)
{
  static const DWORD command_length = 3u;
  static char command_c_string[command_length] = { '\0', '\r', '\n' };

  switch (command)
  {
  case TC_command_t::Activate_data_transmission:
    command_c_string[0] = 'A';
    break;
  case TC_command_t::Stop_data_transmission:
    command_c_string[0] = 'B';
    break;
  case TC_command_t::Celsius_Fahrenheit_key:
    command_c_string[0] = 'C';
    break;
  case TC_command_t::Backlight_key:
    command_c_string[0] = 'D';
    break;
  case TC_command_t::HOLD_key:
    command_c_string[0] = 'E';
    break;
  case TC_command_t::TYPE_key:
    command_c_string[0] = 'F';
    break;
  case TC_command_t::MIN_MAX_key:
    command_c_string[0] = 'G';
    break;
  case TC_command_t::Exit_record_mode:
    command_c_string[0] = 'H';
    break;
  case TC_command_t::T1_key:
    command_c_string[0] = 'I';
    break;
  case TC_command_t::T2_key:
    command_c_string[0] = 'J';
    break;
  case TC_command_t::T1_minus_T2_key:
    command_c_string[0] = 'K';
    break;
  case TC_command_t::REL_key:
    command_c_string[0] = 'L';
    break;
  case TC_command_t::Hi_Lo_LIMITS_key:
    command_c_string[0] = 'M';
    break;
  case TC_command_t::Turn_off_power:
    command_c_string[0] = 'P';
    break;
  default:
    return false;
  }

  DWORD ncomwritten = 0u;
  if (!WriteFile(tc_reader_com_port_handle, command_c_string, command_length, &ncomwritten, NULL))
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    return false;
  }

  if (ncomwritten != command_length)
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    return false;
  }

  return true;
}

/**
 * Clean up function.
 * To be called before exiting normally or abnormally.
 * Should release all acquired resources.
 * Attempts to save output .xlsx file if any valid data
 * was received from the thermocouple reader.
 */
void exit_cleanup(void)
{
  if (log_file_handle.is_open())
  {
    log_file_handle.close();
  }

  if (log_file2_handle.is_open())
  {
    log_file2_handle.close();
  }

  /**
   * Ask the reader to stop sending data.
   */
  if (tc_reader_modified)
  {
    /**
     * Wait in case a command was just sent.
     */
    std::this_thread::sleep_for(std::chrono::milliseconds(intersend_wait_ms));
    send_command(TC_command_t::Stop_data_transmission);
  }

  if (tc_reader_com_port_opened)
  {
    CloseHandle(tc_reader_com_port_handle);
  }

  /**
   * Data saving actually occurs in the exit cleanup function.
   * I might not be a good person.
   */
  if (data_vec.size() > 0u)
  {
    std::printf("Attempting to save data.\n");

    BasicWorkbook::Workbook workbook;
    BasicWorkbook::Sheet &sheet_one = workbook.addSheet("TC Data");
    uint32_t sheet_row = 1u;
    {
      const BasicWorkbook::cell_style_t topleft_wrap_style = {BasicWorkbook::NumberFormat::TEXT, BasicWorkbook::HorizontalAlignment::LEFT, BasicWorkbook::VerticalAlignment::TOP, true, false};
      std::string test_description = "Thermocouple data taken with Omega HH509R reader on " + test_start_date_string + " at " + test_start_time_string + ".";
      sheet_one.add_merged_string_cell("A1", "F1", test_description, topleft_wrap_style);
    }
    sheet_one.set_row_height(1u, 45.0);

    sheet_row = 3u;
    {
      const BasicWorkbook::cell_style_t center_bold_style = {BasicWorkbook::NumberFormat::TEXT, BasicWorkbook::HorizontalAlignment::CENTER, BasicWorkbook::VerticalAlignment::BOTTOM, false, true};
      sheet_one.add_string_cell(sheet_row, 1u, u8"Time (s)", center_bold_style);
      sheet_one.add_string_cell(sheet_row, 2u, u8"T1 Temperature (\u00B0C)", center_bold_style);
      sheet_one.add_string_cell(sheet_row, 3u, u8"T2 Temperature (\u00B0C)", center_bold_style);
      sheet_one.add_string_cell(sheet_row, 4u, u8"TC Type (K/J/T/E/S)", center_bold_style);
      sheet_one.add_string_cell(sheet_row, 5u, u8"Battery (ok/low)", center_bold_style);
    }
  
    sheet_one.set_column_width(1u, 12.505);
    sheet_one.set_column_width(2u, 18.125);
    sheet_one.set_column_width(3u, 18.125);
    sheet_one.set_column_width(4u, 17.755);
    sheet_one.set_column_width(5u, 15.255);

    sheet_row = first_data_row;
    {
      const BasicWorkbook::cell_style_t timestamp_temp_style = {BasicWorkbook::NumberFormat::FIX3, BasicWorkbook::HorizontalAlignment::GENERAL, BasicWorkbook::VerticalAlignment::BOTTOM, false, false};
      for (size_t jSample = 0u; jSample < std::min(data_vec.size(), static_cast<size_t>(BasicWorkbook::MAX_ROW - first_data_row + 1u)); jSample++)
      {
        sheet_one.add_number_cell(sheet_row, 1u, data_vec.at(jSample).timestamp, timestamp_temp_style);
        sheet_one.add_number_cell(sheet_row, 2u, data_vec.at(jSample).T1_degC, timestamp_temp_style);
        sheet_one.add_number_cell(sheet_row, 3u, data_vec.at(jSample).T2_degC, timestamp_temp_style);
        sheet_one.add_string_cell(sheet_row, 4u, std::string(1, TC_type_enum_to_char(data_vec.at(jSample).tc_type)));
        sheet_one.add_string_cell(sheet_row, 5u, data_vec.at(jSample).battery_low ? "low" : "ok");

        sheet_row++;
      }
    }

    bool error = false;
    try
    {
      workbook.publish(workbook_filename);
    }
    catch (std::exception &e)
    {
      std::printf("Error creating workbook file %s.\n%s\n\n", workbook_filename.c_str(), e.what());
      error = true;
    }

    if (!error)
    {
      std::printf("Output file %s saved.\n\n", workbook_filename.c_str());
    }
  }
  else
  {
    std::printf("No valid data received from reader. No output file saved.\n\n");
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

/**
 * Opens COM port and configures it for HH509R thermocouple reader.
 * Returns false on error (should clean up and exit main()) and 
 * returns true on success.
 *
 * Enabling at least one of DTR and RTS is necessary for the
 * thermocouple reader RS232 adapter to function. It uses an optical
 * interface and must draw power from one or both of these pins.
 */
bool open_COM_port(void)
{
  tc_reader_com_port_handle = CreateFile(tc_reader_com_port.c_str(), 
    GENERIC_READ | GENERIC_WRITE,
    0u, NULL, OPEN_EXISTING, 0u, NULL);

  if (tc_reader_com_port_handle == INVALID_HANDLE_VALUE)
  {
    std::printf("Error opening COM port for TC reader, %s.\n\n", tc_reader_com_port_printable.c_str());
    return false;
  }

  tc_reader_com_port_opened = true;

  {
    DCB dcbSerialParams = {};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(tc_reader_com_port_handle, &dcbSerialParams))
    {
      std::printf("Error: Could not retrieve parameters for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
      return false;
    }

    dcbSerialParams.BaudRate          = CBR_1200;
    dcbSerialParams.fBinary           = true;
    dcbSerialParams.fParity           = true;
    dcbSerialParams.fOutxCtsFlow      = false;
    dcbSerialParams.fOutxDsrFlow      = false;
    dcbSerialParams.fDtrControl       = DTR_CONTROL_ENABLE;
    dcbSerialParams.fDsrSensitivity   = false;
    dcbSerialParams.fTXContinueOnXoff = true;
    dcbSerialParams.fOutX             = false;
    dcbSerialParams.fInX              = false;
    dcbSerialParams.fErrorChar        = false;
    dcbSerialParams.fNull             = false;
    dcbSerialParams.fRtsControl       = RTS_CONTROL_ENABLE;
    dcbSerialParams.fAbortOnError     = false;
    dcbSerialParams.wReserved         = 0u;
    dcbSerialParams.ByteSize          = 7u;
    dcbSerialParams.Parity            = EVENPARITY;
    dcbSerialParams.StopBits          = ONESTOPBIT;

    if (!SetCommState(tc_reader_com_port_handle, &dcbSerialParams))
    {
      std::printf("Error: Could not set parameters for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
      return false;
    }
  }

  COMMTIMEOUTS timeouts = {};
  timeouts.ReadIntervalTimeout         = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant    = 0u;
  timeouts.ReadTotalTimeoutMultiplier  = 0u;
  timeouts.WriteTotalTimeoutConstant   = 10u;
  timeouts.WriteTotalTimeoutMultiplier = 10u;

  if (!SetCommTimeouts(tc_reader_com_port_handle, &timeouts))
  {
    std::printf("Error: Could not set timeouts for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    return false;
  }

  return true;
}

/**
 * Main.
 * Parse command line arguments.
 * Open COM port.
 * Ask thermocouple reader to start sending data.
 * Read, validate, parse, store, and print data until Ctrl+C is
 * received, a no data timeout occurs, or we have hit the
 * row limit for a .xlsx workbook file.
 */
int main (int argc, char** argv)
{
  /**
   * Always print the usage message.
   */
  std::printf("Usage:\n");
  std::printf(".\\HH509R_Logger.exe COM_PORT output_file.xlsx\n\n");

  std::printf("COM_PORT should be one of COM1, COM2, COM3, ...\n");
  std::printf("The thermocouple reader should be powered on before running HH509R_Logger.exe.\n");
  std::printf("Key Ctrl+C to end data collection and save the output file.\n\n");

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

  /**
   * Parse COM port argument.
   */
  tc_reader_com_port = argv[1u];
  if (!case_insensitive_same(tc_reader_com_port.substr(0u, 3u), "COM"))
  {
    std::printf("Error: Invalid COM Port Argument. Should be COM1, COM2, COM3, etc.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  {
    bool parse_ok = false;
    int64_t port_number = parse_int64(tc_reader_com_port.substr(3u), parse_ok);
    if (!parse_ok || port_number < 1ll || port_number > 255ll)
    {
      std::printf("Error: Invalid COM Port Argument. COM Port number should be on the range 1-255.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }

    tc_reader_com_port_printable = to_uppercase(tc_reader_com_port);
    if (port_number <= 9ll)
    {
      tc_reader_com_port = tc_reader_com_port_printable;
    }
    else
    {
      tc_reader_com_port = "\\\\.\\" + tc_reader_com_port_printable;
    }
  }

  /**
   * Store output filename.
   * Would be nice to check validity...
   */
  workbook_filename = argv[2u];

  if (!open_COM_port())
  {
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("Opened COM port for TC Reader, %s.\n", tc_reader_com_port_printable.c_str());
  
  /** 
   * Wait for the reader to come online if it, for example
   * uses DTR or RTS to detect a connection.
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(intersend_wait_ms));

  /**
   * Make sure the reader isn't in Record mode.
   */
  if (!send_command(TC_command_t::Exit_record_mode))
  {
    exit_cleanup();
    return EXIT_FAILURE;
  }

  /**
   * Wait so we don't spam the reader with commands.
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(intersend_wait_ms));

  /**
   * In case the thermocouple reader was already sending data, ask it to stop
   * and the purge the RS232 receive buffer. This lets us get a clean start.
   */
  if (!send_command(TC_command_t::Stop_data_transmission))
  {
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("Asked TC Reader to stop sending data.\n");

  /**
   * Wait so that data transmission ends before we clear the RS232 receive
   * buffer. This wait also gives the thermocouple reader time to process
   * the last command before we ask it to start sending data.
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(intersend_wait_ms));
  PurgeComm(tc_reader_com_port_handle, PURGE_RXCLEAR);
  
  /**
   * Ask the thermocouple reader to start sending data.
   * Set tc_reader_modified to true so we ask the thermocouple reader
   * to stop sending data in the exit / cleanup routine.
   */
  tc_reader_modified = true;
  if (!send_command(TC_command_t::Activate_data_transmission))
  {
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("Asked TC Reader to start sending data.\n\n");

  /**
   * Get the date and time of test start just after
   * asking the thermocouple reader to start sending data.
   *
   * Times based on system_clock are purely marking the calendar
   * date and time of test start in the output file header.
   *
   * Times based on steady_clock are used for timestamping
   * measurements in units of seconds from time of test start.
   */
  {
    std::tm timestruct = IttyZip::localtime_locked(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    const size_t TIME_BUF_SIZE = 300u;
    char time_date_string_buf[TIME_BUF_SIZE];

    if (0u == strftime(time_date_string_buf, TIME_BUF_SIZE, "%Y-%m-%d", &timestruct))
    {
      std::printf("Could not assemble date string for output .xlsx file header.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }
    test_start_date_string = std::string(time_date_string_buf);

    if (0u == strftime(time_date_string_buf, TIME_BUF_SIZE, "%H:%M:%S %Z", &timestruct))
    {
      std::printf("Could not assemble time string for output .xlsx file header.\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }
    test_start_time_string = std::string(time_date_string_buf);
  }

  /**
   * Using the steady_clock for data collection timestamps.
   *
   * last_data_time and last_valid_data_time are for timeouts to end
   * collection if the thermocouple reader stops sending any data or
   * stops sending valid data.
   *
   * last_send_time is used to enforce delays between sent commands
   * so we don't spam the thermocouple reader
   */
  std::chrono::steady_clock::time_point test_start_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_data_time = test_start_time;
  std::chrono::steady_clock::time_point last_valid_data_time = test_start_time;
  std::chrono::steady_clock::time_point last_send_time = test_start_time;
  std::chrono::steady_clock::time_point last_record_mode_change_time = test_start_time;
  std::chrono::steady_clock::time_point last_rel_mode_change_time = test_start_time;
  std::chrono::steady_clock::time_point last_hold_mode_change_time = test_start_time;
  std::chrono::steady_clock::time_point last_limits_mode_change_time = test_start_time;
  std::chrono::steady_clock::time_point last_restart_time = test_start_time;

  /**
   * read_data serves as a buffer for data received over the COM port
   * before even the first step of validation or parsing.
   */
  std::string read_data;

  /**
   * The thermocouple reader sends each data line three times.
   *
   * line_check_queue is used to check for two exactly identical data lines
   * in the last three received data lines of valid length to prevent
   * corrupted data from being stored.
   */
  std::deque<sample_and_timestamp_t> line_check_queue;

  /**
   * These temperature variables and first_sample are used
   * for plotting temperature data to the console during collection.
   */
  double T1_min_degC = std::numeric_limits<double>::max();
  double T1_max_degC = std::numeric_limits<double>::min();
  double T2_min_degC = std::numeric_limits<double>::max();
  double T2_max_degC = std::numeric_limits<double>::min();
  double T1_T2_min_degC = std::numeric_limits<double>::max();
  double T1_T2_max_degC = std::numeric_limits<double>::min();

  log_file_handle.open("log.txt", std::ios::out | std::ios::binary | std::ios::trunc);
  if (!log_file_handle.is_open())
  {
    std::printf("could not open log file for debugging.\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  log_file2_handle.open("log2.txt", std::ios::out | std::ios::binary | std::ios::trunc);
  if (!log_file2_handle.is_open())
  {
    std::printf("could not open log file 2 for debugging.\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  /**
   * Receive thermocouple reader data in a loop that only terminates
   * after a Ctrl+C signal, 60 seconds of no data reception of any kind,
   * 300 seconds of no valid data reception, or enough collected data
   * to hit the row limit of a .xlsx workbook file.
   */
  while (true)
  {
    if (ctrl_c_happened)
    {
      std::printf("Received Ctrl+C signal. Ending data collection.\n\n");
      exit_cleanup();
      return EXIT_SUCCESS;
    }

    std::chrono::steady_clock::time_point present_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration no_data_timeout_check = present_time - last_data_time;
    std::chrono::steady_clock::duration no_valid_data_timeout_check = present_time - last_valid_data_time;

    /**
     * If the thermocouple reader hasn't been sending data or hasn't
     * been sending valid data, close the COM port, wait, reopen the
     * COM port, and then ask it to start sending data again.
     *
     * Depending on when the thermocouple reader is powered on relative
     * to when the COM port is opened, this might not restart data
     * collection.
     */
    if ( (std::chrono::seconds(restart_retry_s) < no_data_timeout_check ||
          std::chrono::seconds(restart_retry_s) < no_valid_data_timeout_check) &&
         std::chrono::seconds(restart_retry_s) < (present_time - last_restart_time) && 
         std::chrono::milliseconds(intersend_wait_ms) < (present_time - last_send_time) )
    {
      CloseHandle(tc_reader_com_port_handle);
      
      std::this_thread::sleep_for(std::chrono::seconds(restart_wait_s));

      if (!open_COM_port())
      {
        exit_cleanup();
        return EXIT_FAILURE;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(intersend_wait_ms));

      if (!send_command(TC_command_t::Activate_data_transmission))
      {
        exit_cleanup();
        return EXIT_FAILURE;
      }

      last_send_time = std::chrono::steady_clock::now();
      last_restart_time = last_send_time;
    }

    if (std::chrono::seconds(no_data_timeout_s) < no_data_timeout_check)
    {
      std::printf("Received no data for %.3f seconds. Ending data collection.\n", std::chrono::duration<double>(no_data_timeout_check).count());
      std::printf("Did the thermocouple reader turn off?\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }

    if (std::chrono::seconds(no_valid_data_timeout_s) < no_valid_data_timeout_check)
    {
      std::printf("Received no valid data for %.3f seconds. Ending data collection.\n", std::chrono::duration<double>(no_valid_data_timeout_check).count());
      std::printf("Did the thermocouple reader turn off?\n\n");
      exit_cleanup();
      return EXIT_FAILURE;
    }

    DWORD ncomread = 0u;
    if (ReadFile(tc_reader_com_port_handle, read_buffer, static_cast<DWORD>(read_buf_size), &ncomread, NULL) &&
        ncomread > 0u)
    {
      last_data_time = std::chrono::steady_clock::now();
      log_file_handle.write(read_buffer, ncomread);
      log_file_handle << std::flush;
      read_data.append(read_buffer, ncomread);
      log_file2_handle << "\r\n...\r\n" << read_data << "\r\n...\r\n" << std::flush;
      size_t line_end_pos = 0;
      line_end_pos = read_data.find("\r\n", 0u);

      while (line_end_pos != std::string::npos)
      {
        /**
         * A proper data line will have 30 bytes followed by \r\n.
         */
        if (line_end_pos >= valid_data_line_length)
        {
          present_time = std::chrono::steady_clock::now();
          sample_and_timestamp_t read_struct;
          read_struct.sample_string = read_data.substr(line_end_pos - valid_data_line_length, valid_data_line_length);
          read_struct.sample_time_s = std::chrono::duration<double>(present_time - test_start_time).count();
          line_check_queue.push_back(read_struct);

          /**
           * The thermocouple reader sends each data line 3 times.
           * Check the last three lines for at least two that are the same and
           * only store data when that is found.
           * This avoids storing data that was corrupted during transmission,
           * which does happen with this device.
           */
          while (line_check_queue.size() > transmissions_per_sample)
          {
            line_check_queue.pop_front();
          }

          if (line_check_queue.size() == transmissions_per_sample)
          {
            bool store_sample = false;
            uint8_t lines_to_clear = 1u;

            if (line_check_queue.at(0u).sample_string.compare(line_check_queue.at(1u).sample_string) == 0 &&
                line_check_queue.at(1u).sample_string.compare(line_check_queue.at(2u).sample_string) == 0)
            {
              /**
               * All three data lines match. Clear all three.
               */
              store_sample = true;
              lines_to_clear = transmissions_per_sample;
            }
            else if (line_check_queue.at(0u).sample_string.compare(line_check_queue.at(2u).sample_string) == 0)
            {
              /**
               * The first and third data lines match; the second is bad.
               * Clear all three data lines.
               */
              store_sample = true;
              lines_to_clear = transmissions_per_sample;
            }
            else if (line_check_queue.at(0u).sample_string.compare(line_check_queue.at(1u).sample_string) == 0)
            {
              /**
               * The first and second data lines match.
               * Either the third line is bad OR the first line was bad and got pushed out
               * by new data.
               * Clear the first two lines.
               */
              store_sample = true;
              lines_to_clear = 2u;
            }
            else if (line_check_queue.at(1u).sample_string.compare(line_check_queue.at(2u).sample_string) == 0)
            {
              /**
               * The second and third line match. Only clear the first line.
               * This sample will be stored in the next iteration (assuming more valid data arrives).
               */
              store_sample = false;
              lines_to_clear = 1u;
            }
            else
            {
              /**
               * No matches. I have no idea. Only clear the first line.
               */
              store_sample = false;
              lines_to_clear = 1u;
            }

            if (store_sample)
            {
              std::string sample = line_check_queue.at(0u).sample_string;
                
              /**
               * Check the sample string for any errors discoverable by
               * simply looking at the allowed character ranges.
               */
              bool any_errors = false;

              any_errors |= (sample.at(T1_temp_sign_index) != '+' && 
                             sample.at(T1_temp_sign_index) != '-');

              for (size_t jChar = T1_temp_MSD_index; jChar <= T1_temp_LSD_index; jChar++)
              {
                any_errors |= (!is_hex_char(sample.at(jChar)));
              }

              any_errors |= (sample.at(TC_type_char_index) != 'K' && 
                             sample.at(TC_type_char_index) != 'J' && 
                             sample.at(TC_type_char_index) != 'T' && 
                             sample.at(TC_type_char_index) != 'E' && 
                             sample.at(TC_type_char_index) != 'S');

              any_errors |= (sample.at(T2_temp_sign_index) != '+' &&
                             sample.at(T2_temp_sign_index) != '-');
                
              for (size_t jChar = T2_temp_MSD_index; jChar <= T2_temp_LSD_index; jChar++)
              {
                any_errors |= (!is_hex_char(sample.at(jChar)));
              }
                
              any_errors |= (sample.at(Unused_char_1_index) != '_');
                
              for (size_t jChar = Hours_MSD_index; jChar <= Hours_LSD_index; jChar++)
              {
                any_errors |= (!isdigit(sample.at(jChar)));
              }
              
              /**
               * Past 9 hours, 59 minutes, and 59 seconds internal HH509R timer duration,
               * the most significant minutes digit becomes ascii value 29.
               *
               * Since we're not using device timestamps anyway, allow this.
               */
              any_errors |= (sample.at(Minutes_MSD_index) != '0' && 
                             sample.at(Minutes_MSD_index) != '1' && 
                             sample.at(Minutes_MSD_index) != '2' && 
                             sample.at(Minutes_MSD_index) != '3' && 
                             sample.at(Minutes_MSD_index) != '4' && 
                             sample.at(Minutes_MSD_index) != '5' && 
                             sample.at(Minutes_MSD_index) != '6' && 
                             sample.at(Minutes_MSD_index) != static_cast<char>(29));


              /**
               * Continue to treat ascii value 29 in the most significant minutes digit
               * as invalid before the 10 hour mark.
               */
              any_errors |= (sample.at(Hours_MSD_index) == '0' &&
                             sample.at(Minutes_MSD_index) == static_cast<char>(29));

              for (size_t jChar = Minutes_LSD_index; jChar <= Seconds_LSD_index; jChar++)
              {
                any_errors |= (!isdigit(sample.at(jChar)));
              }
                
              any_errors |= (sample.at(Seconds_MSD_index) == '7' || 
                             sample.at(Seconds_MSD_index) == '8' || 
                             sample.at(Seconds_MSD_index) == '9');
                
              any_errors |= (sample.at(Record_mode_char_index) != 'R' && 
                             sample.at(Record_mode_char_index) != 'M' && 
                             sample.at(Record_mode_char_index) != 'I' && 
                             sample.at(Record_mode_char_index) != 'A' && 
                             sample.at(Record_mode_char_index) != '-' && 
                             sample.at(Record_mode_char_index) != '_');
                
              any_errors |= (sample.at(Rel_hold_mode_char_index) != 'R' && 
                             sample.at(Rel_hold_mode_char_index) != 'H' && 
                             sample.at(Rel_hold_mode_char_index) != '_');

              any_errors |= (sample.at(Unused_char_2_index) != '_');

              any_errors |= (sample.at(Limits_mode_char_index) != 'L' &&
                             sample.at(Limits_mode_char_index) != '_');

              any_errors |= (sample.at(Hi_limit_char_index) != 'H' &&
                             sample.at(Hi_limit_char_index) != '_');

              any_errors |= (sample.at(Lo_limit_char_index) != 'L' && 
                             sample.at(Lo_limit_char_index) != '_');

              any_errors |= (sample.at(Unused_char_3_index) != '_');
              
              any_errors |= (sample.at(Battery_status_char_index) != 'B' &&
                             sample.at(Battery_status_char_index) != '_');

              if (!any_errors)
              {
                /**
                 * Data looks basically valid. Parse it into a struct,
                 * save in data_vec for later storage in a .xlsx file,
                 * and print the readings so the user can see what's
                 * going on.
                 */
                last_valid_data_time = std::chrono::steady_clock::now();

                parsed_sample_t parsed_sample;

                /**
                 * Thermocouple reader timestamps appear to be unreliable from
                 * streamed data that I have viewed.
                 * Use the steady_clock timestamp of data reception to mark
                 * the measurement time of each sample.
                 * I suppose this can be late by about 1/3 of a second in the
                 * case that the first data line has a communication error.
                 */
                parsed_sample.timestamp = line_check_queue.at(0u).sample_time_s;
                  
                /**
                 * Convert temperature readings from sign/magnitude hexadecimal integer
                 * millidegrees Celsius to double floating point degrees Celsius.
                 */
                parsed_sample.T1_degC = signmag_hexint_mdegC_to_degC(sample, T1_temp_sign_index, T1_temp_MSD_index);
                parsed_sample.T2_degC = signmag_hexint_mdegC_to_degC(sample, T2_temp_sign_index, T2_temp_MSD_index);

                /**
                 * Thermocouple type character.
                 */
                switch (sample.at(TC_type_char_index))
                {
                case 'K': 
                  parsed_sample.tc_type = TC_type_t::K_type;
                  break;
                case 'J': 
                  parsed_sample.tc_type = TC_type_t::J_type;
                  break;
                case 'T': 
                  parsed_sample.tc_type = TC_type_t::T_type;
                  break;
                case 'E': 
                  parsed_sample.tc_type = TC_type_t::E_type;
                  break;
                case 'S': 
                  parsed_sample.tc_type = TC_type_t::S_type;
                  break;
                default: 
                  std::printf("Error: Found invalid thermocouple type data after validation.\n\n");
                  exit_cleanup();
                  return EXIT_FAILURE;
                }

                /**
                 * Record mode character.
                 */
                switch (sample.at(Record_mode_char_index))
                {
                case 'R':
                  parsed_sample.record_mode = Record_mode_t::Record;
                  break;
                case 'M':
                  parsed_sample.record_mode = Record_mode_t::Max;
                  break;
                case 'I':
                  parsed_sample.record_mode = Record_mode_t::Min;
                  break;
                case 'A':
                  parsed_sample.record_mode = Record_mode_t::Average;
                  break;
                case '-':
                  parsed_sample.record_mode = Record_mode_t::Range;
                  break;
                case '_':
                  parsed_sample.record_mode = Record_mode_t::Normal;
                  break;
                default:
                  std::printf("Error: Found invalid record mode data after validation.\n\n");
                  exit_cleanup();
                  return EXIT_FAILURE;
                }

                /**
                 * Rel/Hold mode character, which is interesting
                 * since both may be set.
                 */
                switch (sample.at(Rel_hold_mode_char_index))
                {
                case 'R':
                  parsed_sample.rel_hold_mode = Rel_Hold_mode_t::Rel;
                  break;
                case 'H':
                  parsed_sample.rel_hold_mode = Rel_Hold_mode_t::Hold;
                  break;
                case '_':
                  parsed_sample.rel_hold_mode = Rel_Hold_mode_t::Normal;
                  break;
                default:
                  std::printf("Error: Found invalid Rel / Hold mode data after validation.\n\n");
                  exit_cleanup();
                  return EXIT_FAILURE;
                }

                /**
                 * Limits mode character.
                 */
                if (sample.at(Limits_mode_char_index) == 'L')
                {
                  parsed_sample.limits_mode = true;
                }
                else
                {
                  parsed_sample.limits_mode = false;
                }

                /**
                 * Battery status character.
                 */
                if (sample.at(Battery_status_char_index) == 'B')
                {
                  parsed_sample.battery_low = true;
                }
                else
                {
                  parsed_sample.battery_low = false;
                }

                /**
                 * Parsed data line stored for recording in output file
                 * upon exit.
                 */
                data_vec.push_back(parsed_sample);

                /**
                 * All of this is just printing temperature measurements during
                 * data collection so the user can see what's going on.
                 */
                T1_min_degC = std::min(T1_min_degC, parsed_sample.T1_degC);
                T1_max_degC = std::max(T1_max_degC, parsed_sample.T1_degC);
                T2_min_degC = std::min(T2_min_degC, parsed_sample.T2_degC);
                T2_max_degC = std::max(T2_max_degC, parsed_sample.T2_degC);
                double T1_T2_degC = parsed_sample.T1_degC - parsed_sample.T2_degC;
                T1_T2_min_degC = std::min(T1_T2_min_degC, T1_T2_degC);
                T1_T2_max_degC = std::max(T1_T2_max_degC, T1_T2_degC);

                std::printf("-    T1 now/min/max deg. C: %9.3f / %9.3f / %9.3f, T2 now/min/max deg. C: %9.3f / %9.3f / %9.3f\n", parsed_sample.T1_degC, T1_min_degC, T1_max_degC, parsed_sample.T2_degC, T2_min_degC, T2_max_degC);
                std::printf("  T1-T2 now/min/max deg. C: %9.3f / %9.3f / %9.3f, TC Type: %c, Batt: %s, Time: %.3fs\n", T1_T2_degC, T1_T2_min_degC, T1_T2_max_degC, TC_type_enum_to_char(parsed_sample.tc_type), parsed_sample.battery_low ? "low" : "ok", parsed_sample.timestamp);

                size_t jSample = data_vec.size() - 1u;
                double T1_T2_lookback_min = std::numeric_limits<double>::max();
                double T1_T2_lookback_max = std::numeric_limits<double>::min();
                while (true)
                {
                  T1_T2_lookback_min = std::min(T1_T2_lookback_min, data_vec.at(jSample).T1_degC - data_vec.at(jSample).T2_degC);
                  T1_T2_lookback_max = std::max(T1_T2_lookback_max, data_vec.at(jSample).T1_degC - data_vec.at(jSample).T2_degC);
                    
                  if (min_T1_T2_lookback_s <= parsed_sample.timestamp - data_vec.at(jSample).timestamp)
                  {
                    std::printf("        T1-T2 deg. C range: %9.3f in last %.3fs\n", T1_T2_lookback_max - T1_T2_lookback_min, parsed_sample.timestamp - data_vec.at(jSample).timestamp);
                    break;
                  }

                  if (jSample == 0u) break;
                  jSample--;
                }

                std::printf("\n");

                /**
                 * There is no point in continuing to collect data that we
                 * can't save in the output file.
                 *
                 * This MIGHT occur if the user has wired a long-lasting
                 * 9V source into the battery connector of their thermocoupler reader.
                 */
                if (data_vec.size() >= static_cast<size_t>(BasicWorkbook::MAX_ROW - first_data_row + 1u))
                {
                  std::printf("Amount of collected data has reached the .xlsx workbook file maximum row limit, %u.\n", BasicWorkbook::MAX_ROW);
                  std::printf("Stopping data collection.\n\n");
                  exit_cleanup();
                  return EXIT_SUCCESS;
                }

                /**
                 * This bit of code attempts to keep the reader out of Record mode.
                 * The retry wait prevents oscillation into and out of Record mode.
                 * The intersend wait prevents spamming the reader with commands.
                 */
                if (std::chrono::milliseconds(mode_change_retry_ms) < (present_time - last_record_mode_change_time) &&
                    std::chrono::milliseconds(intersend_wait_ms) < (present_time - last_send_time) &&
                    parsed_sample.record_mode != Record_mode_t::Normal)
                {
                  last_send_time = present_time;
                  last_record_mode_change_time = present_time;

                  if (!send_command(TC_command_t::Exit_record_mode))
                  {
                    exit_cleanup();
                    return EXIT_FAILURE;
                  }
                }

                /**
                 * This bit of code attempts to keep the reader out of Limits mode.
                 * The retry wait prevents oscillation into and out of Limits mode.
                 * The intersend wait prevents spamming the reader with commands.
                 */
                if (std::chrono::milliseconds(mode_change_retry_ms) < (present_time - last_limits_mode_change_time) &&
                    std::chrono::milliseconds(intersend_wait_ms) < (present_time - last_send_time) &&
                    parsed_sample.limits_mode)
                {
                  last_send_time = present_time;
                  last_limits_mode_change_time = present_time;

                  if (!send_command(TC_command_t::Hi_Lo_LIMITS_key))
                  {
                    exit_cleanup();
                    return EXIT_FAILURE;
                  }
                }

                /**
                 * This bit of code attempts to keep the reader out of Hold mode.
                 * The retry wait prevents oscillation into and out of Hold mode.
                 * The intersend wait prevents spamming the reader with commands.
                 */
                if (std::chrono::milliseconds(mode_change_retry_ms) < (present_time - last_hold_mode_change_time) &&
                    std::chrono::milliseconds(intersend_wait_ms) < (present_time - last_send_time) &&
                    parsed_sample.rel_hold_mode == Rel_Hold_mode_t::Hold)
                {
                  last_send_time = present_time;
                  last_hold_mode_change_time = present_time;

                  if (!send_command(TC_command_t::HOLD_key))
                  {
                    exit_cleanup();
                    return EXIT_FAILURE;
                  }
                }

                /**
                 * This bit of code attempts to keep the reader out of Rel mode.
                 * The retry wait prevents oscillation into and out of Rel mode.
                 * The intersend wait prevents spamming the reader with commands.
                 */
                if (std::chrono::milliseconds(mode_change_retry_ms) < (present_time - last_rel_mode_change_time) &&
                    std::chrono::milliseconds(intersend_wait_ms) < (present_time - last_send_time) &&
                    parsed_sample.rel_hold_mode == Rel_Hold_mode_t::Rel)
                {
                  last_send_time = present_time;
                  last_rel_mode_change_time = present_time;

                  if (!send_command(TC_command_t::REL_key))
                  {
                    exit_cleanup();
                    return EXIT_FAILURE;
                  }
                }
              }
            }

            /**
             * Clear data lines from the queue that no longer need to be evaluated.
             */
            for (uint8_t jClear = 0u; jClear < lines_to_clear; jClear++)
            {
              line_check_queue.pop_front();
            }
          }
        }

        /**
         * Erase the read data up through the first \r\n.
         * Either it was valid and already parsed or it had an invalid length.
         */
        if (read_data.size() > line_end_pos + line_terminator_length)
        {
          read_data = read_data.substr(line_end_pos + line_terminator_length);
        }
        else
        {
          read_data.clear();
        }
        log_file2_handle << "\r\n---\r\n" << read_data << "\r\n---\r\n" << std::flush;

        line_end_pos = read_data.find("\r\n", 0u);
      }
    }

    /**
     * Waiting 10ms between attempts at reading serial port
     * so as not to consume too many resources.
     * This is plenty fast for 32 bytes 3 times per second
     * at 1200 baud.
     */
    std::this_thread::sleep_for(std::chrono::milliseconds(poll_min_wait_ms));
  }

  /**
   * This code should not be reached.
   */
  exit_cleanup();
  return EXIT_FAILURE;
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
