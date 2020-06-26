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
* The program ceases data collection and saves outputs under two conditions:
*   1. The user keys Ctrl+C
*   2. No data is received from the thermocouple reader for 30 seconds.
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
#define NOMINMAX
#include <Windows.h>
#include <WinBase.h>
#include "parser_helpers.h"
#include "BasicWorkbook.h"

/**
 * Types
 */
typedef struct
{
  std::string sample_string;
  double sample_time_s;
} sample_and_timestamp_t;

enum class TC_type_t : uint8_t
{
  K_type,
  J_type,
  T_type,
  E_type,
  S_type
};

enum class Record_mode_t : uint8_t
{
  Record,
  Max,
  Min,
  Average,
  Range,
  Normal
};

enum class Rel_Hold_mode_t : uint8_t
{
  Rel,
  Hold,
  Normal
};

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
 * Globals
 */
bool ctrl_c_happened = false;
HANDLE tc_reader_com_port_handle;
bool tc_reader_com_port_opened = false;
std::string tc_reader_com_port;
std::string tc_reader_com_port_printable;
bool tc_reader_modified = false;
const size_t write_buf_size = 4u;
char write_buffer[write_buf_size];
const size_t read_buf_size = 320u;
char read_buffer[read_buf_size];
std::string test_start_date;
std::string test_start_time;
std::vector<parsed_sample_t> data_vec;
BasicWorkbook::Workbook workbook;
std::string workbook_filename;

/**
 * Clean up function.
 * To be called before exiting normally or abnormally.
 * Should release all acquired resources.
 */
void exit_cleanup(void)
{
  /**
   * Attempt to get the themocouple out of record mode and
   * also ask it to stop sending data.
   */
  if (tc_reader_modified)
  {
    strncpy(write_buffer, "H\r\n", write_buf_size);
    DWORD ncomwritten = 0u;
    if (!WriteFile(tc_reader_com_port_handle, write_buffer, static_cast<DWORD>(write_buf_size-1u), &ncomwritten, NULL))
    {
      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    }

    if (ncomwritten != static_cast<DWORD>(write_buf_size-1u))
    {
      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    strncpy(write_buffer, "B\r\n", write_buf_size);
    ncomwritten = 0u;
    if (!WriteFile(tc_reader_com_port_handle, write_buffer, static_cast<DWORD>(write_buf_size-1u), &ncomwritten, NULL))
    {
      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    }

    if (ncomwritten != static_cast<DWORD>(write_buf_size-1u))
    {
      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    }
  }

  if (tc_reader_com_port_opened)
  {
    CloseHandle(tc_reader_com_port_handle);
  }

  /**
   * Data saving actually occurs in the exit cleanup function.
   * I might not be a good person.
   */
  if (data_vec.size() > 0)
  {
    std::printf("Attempting to save data.\n");

    BasicWorkbook::Sheet &sheet_one = workbook.addSheet("TC Data");
    uint32_t sheet_row = 1u;
    BasicWorkbook::cell_style_t topleft_wrap_style = {BasicWorkbook::NumberFormat::TEXT, BasicWorkbook::HorizontalAlignment::LEFT, BasicWorkbook::VerticalAlignment::TOP, true, false};
    std::string test_description = "Thermocouple data taken with Omega HH509R reader on " + test_start_date + " at " + test_start_time + ".";
    sheet_one.add_merged_string_cell("A1", "F1", test_description, topleft_wrap_style);
    sheet_one.set_row_height(1, 45.0);

    sheet_row = 3u;
    sheet_one.add_string_cell(sheet_row, 1u, u8"Time (s)");
    sheet_one.add_string_cell(sheet_row, 2u, u8"T1 Temperature (\u00B0C)");
    sheet_one.add_string_cell(sheet_row, 3u, u8"T2 Temperature (\u00B0C)");
    sheet_one.add_string_cell(sheet_row, 4u, u8"TC Type (K/J/T/E/S)");
    sheet_one.add_string_cell(sheet_row, 5u, u8"Battery (ok/low)");
    sheet_row++;
  
    sheet_one.set_column_width(2, 17.505);
    sheet_one.set_column_width(3, 17.505);
    sheet_one.set_column_width(4, 16.755);
    sheet_one.set_column_width(5, 14.125);

    BasicWorkbook::cell_style_t timestamp_temp_style = {BasicWorkbook::NumberFormat::FIX3, BasicWorkbook::HorizontalAlignment::GENERAL, BasicWorkbook::VerticalAlignment::BOTTOM, false, false};

    for (size_t jSample = 0; jSample < data_vec.size(); jSample++)
    {
      sheet_one.add_number_cell(sheet_row, 1u, data_vec.at(jSample).timestamp, timestamp_temp_style);
      sheet_one.add_number_cell(sheet_row, 2u, data_vec.at(jSample).T1_degC, timestamp_temp_style);
      sheet_one.add_number_cell(sheet_row, 3u, data_vec.at(jSample).T2_degC, timestamp_temp_style);
    
      std::string tc_type_string;
    
      switch (data_vec.at(jSample).tc_type)
      {
      case TC_type_t::K_type:
        tc_type_string = "K";
        break;
      case TC_type_t::J_type:
        tc_type_string = "J";
        break;
      case TC_type_t::T_type:
        tc_type_string = "T";
        break;
      case TC_type_t::E_type:
        tc_type_string = "E";
        break;
      case TC_type_t::S_type:
        tc_type_string = "S";
        break;
      default:
        tc_type_string = "K";
        break;
      }

      sheet_one.add_string_cell(sheet_row, 4u, tc_type_string);
      sheet_one.add_string_cell(sheet_row, 5u, data_vec.at(jSample).battery_low ? "low" : "ok");

      sheet_row++;
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

int main (int argc, char** argv)
{
  std::printf("Usage:\n\n");
  std::printf(".\\HH509R_Logger.exe COM_PORT output_file.xlsx\n\n");
  std::printf("Key Ctrl+C to end data collection and save.\n\n");

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

  std::string com_port_arg = argv[1];
  if (!case_insensitive_same(com_port_arg.substr(0u, 3u), "COM"))
  {
    std::printf("Error: Invalid COM Port Argument. Should be COM1, COM2, COM3, etc.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  bool parse_ok = false;
  int64_t port_number = parse_int64(com_port_arg.substr(3u), parse_ok);
  if (!parse_ok || port_number < 1ll || port_number > 255ll)
  {
    std::printf("Error: Invalid COM Port Argument. COM Port number should be on the range 1-255.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }

  tc_reader_com_port_printable = to_uppercase(com_port_arg);
  if (port_number <= 9ll)
  {
    tc_reader_com_port = tc_reader_com_port_printable;
  }
  else
  {
    tc_reader_com_port = "\\\\.\\" + tc_reader_com_port_printable;
  }

  workbook_filename = argv[2];

  tc_reader_com_port_handle = CreateFile(tc_reader_com_port.c_str(), 
    GENERIC_READ | GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL);

  if (tc_reader_com_port_handle == INVALID_HANDLE_VALUE)
  {
    std::printf("Error opening COM port for TC reader, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  tc_reader_com_port_opened = true;

  DCB dcbSerialParams = { 0 };
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  if (!GetCommState(tc_reader_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not retrieve parameters for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
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
  dcbSerialParams.wReserved         = 0;
  dcbSerialParams.ByteSize          = 7;
  dcbSerialParams.Parity            = EVENPARITY;
  dcbSerialParams.StopBits          = ONESTOPBIT;

  if (!SetCommState(tc_reader_com_port_handle, &dcbSerialParams))
  {
    std::printf("Error: Could not set parameters for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  COMMTIMEOUTS timeouts = { 0 };
  timeouts.ReadIntervalTimeout         = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant    = 0;
  timeouts.ReadTotalTimeoutMultiplier  = 0;
  timeouts.WriteTotalTimeoutConstant   = 10;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  if (!SetCommTimeouts(tc_reader_com_port_handle, &timeouts))
  {
    std::printf("Error: Could not set timeouts for TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }  

  std::printf("Opened COM port for TC Reader, %s.\n", tc_reader_com_port_printable.c_str());

  /**
   * In case the thermocouple reader was already sending data, ask it to stop
   * and the purge the RS232 receive buffer. This lets us get a clean start.
   */
  strncpy(write_buffer, "B\r\n", write_buf_size);
  DWORD ncomwritten = 0u;
  if (!WriteFile(tc_reader_com_port_handle, write_buffer, static_cast<DWORD>(write_buf_size-1u), &ncomwritten, NULL))
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (ncomwritten != static_cast<DWORD>(write_buf_size-1u))
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("Asked TC Reader to stop sending data.\n");

  /**
   * Wait for data transmission to end before clearing RS232 receive buffer.
   * This wait also gives the thermocouple reader time to recover before
   * we ask it to start sending data.
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  PurgeComm(tc_reader_com_port_handle, PURGE_RXCLEAR);
  
  /**
   * Ask the thermocouple reader to start sending data.
   */
  tc_reader_modified = true;
  strncpy(write_buffer, "A\r\n", write_buf_size);
  ncomwritten = 0u;
  if (!WriteFile(tc_reader_com_port_handle, write_buffer, static_cast<DWORD>(write_buf_size-1u), &ncomwritten, NULL))
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  if (ncomwritten != static_cast<DWORD>(write_buf_size-1u))
  {
    std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
    exit_cleanup();
    return EXIT_FAILURE;
  }

  std::printf("Asked TC Reader to start sending data.\n\n");

  /**
  * Get the date and time of test start just after
  * asking the thermocouple reader to start sending data.
  */
  std::time_t timepoint = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm timestruct = IttyZip::localtime_locked(timepoint);
  const size_t TIME_BUF_SIZE = 300u;
  char timestamp[TIME_BUF_SIZE];

  size_t retval = strftime(timestamp, TIME_BUF_SIZE, "%Y-%m-%d", &timestruct);
  if (retval == 0u)
  {
    std::printf("Could not assemble date string for output .xlsx file header.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  test_start_date = std::string(timestamp);

  retval = strftime(timestamp, TIME_BUF_SIZE, "%H:%M:%S %Z", &timestruct);
  if (retval == 0u)
  {
    std::printf("Could not assemble time string for output .xlsx file header.\n\n");
    exit_cleanup();
    return EXIT_FAILURE;
  }
  test_start_time = std::string(timestamp);

  /**
   * Using the steady clock for data collection timestamps.
   * This is handy for graphing and so forth.
   */
  std::chrono::steady_clock::time_point test_start_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_data_time = test_start_time;
  std::chrono::steady_clock::time_point last_send_time = test_start_time;
  std::string read_data;
  std::deque<sample_and_timestamp_t> line_check_queue;
  double T1_min_degC = std::numeric_limits<double>::quiet_NaN();
  double T1_max_degC = std::numeric_limits<double>::quiet_NaN();
  double T2_min_degC = std::numeric_limits<double>::quiet_NaN();
  double T2_max_degC = std::numeric_limits<double>::quiet_NaN();
  double T1_T2_min_degC = std::numeric_limits<double>::quiet_NaN();
  double T1_T2_max_degC = std::numeric_limits<double>::quiet_NaN();
  bool first_sample = true;

  /**
   * Receive thermocouple reader data in a loop that only terminates
   * after 30 seconds of no received data or a Ctrl+C signal.
   */
  while (true)
  {
    if (ctrl_c_happened)
    {
      std::printf("Received Ctrl+C signal. Ending data collection.\n\n");
      break;
    }

    if (std::chrono::seconds(30) < (std::chrono::steady_clock::now() - last_data_time))
    {
      std::printf("Received no data for 30 seconds. Ending data collection.\n");
      std::printf("Did the thermocouple reader turn off?\n\n");
      break;
    }

    DWORD ncomread = 0u;
    if (ReadFile(tc_reader_com_port_handle, read_buffer, static_cast<DWORD>(read_buf_size), &ncomread, NULL))
    {
      if (ncomread > 0u)
      {
        last_data_time = std::chrono::steady_clock::now();
        read_data.append(read_buffer, ncomread);
        size_t line_end_pos = read_data.find("\r\n", 0);

        if (line_end_pos != std::string::npos)
        {
          /**
           * A proper data line will have 30 bytes followed by \r\n.
           */
          if (line_end_pos == 30)
          {
            std::chrono::steady_clock::time_point present_time = std::chrono::steady_clock::now();
            sample_and_timestamp_t read_struct;
            read_struct.sample_string = read_data.substr(0,30);
            read_struct.sample_time_s = std::chrono::duration<double>(present_time - test_start_time).count();
            line_check_queue.push_back(read_struct);

            /**
             * The thermocouple reader sends each data line 3 times. So only bother checking
             * the last three lines for at least two that are the same.
             */
            if (line_check_queue.size() > 3)
            {
              line_check_queue.pop_front();
            }

            if (line_check_queue.size() == 3)
            {
              bool store_sample = false;
              uint8_t lines_to_clear = 1u;

              if (line_check_queue.at(0).sample_string.compare(line_check_queue.at(1).sample_string) == 0 &&
                  line_check_queue.at(1).sample_string.compare(line_check_queue.at(2).sample_string) == 0)
              {
                /* All three data lines match. */
                store_sample = true;
                lines_to_clear = 3u;
              }
              else if (line_check_queue.at(0).sample_string.compare(line_check_queue.at(2).sample_string) == 0)
              {
                /* The first and third data lines match; the second is bad. */
                store_sample = true;
                lines_to_clear = 3u;
              }
              else if (line_check_queue.at(0).sample_string.compare(line_check_queue.at(1).sample_string) == 0)
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
              else if (line_check_queue.at(1).sample_string.compare(line_check_queue.at(2).sample_string) == 0)
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
                /* No matches. I have no idea. Only clear the first line. */
                store_sample = false;
                lines_to_clear = 1u;
              }

              if (store_sample)
              {
                std::string sample = line_check_queue.at(0).sample_string;
                
                /**
                 * Check the sample string for any errors discoverable by
                 * simply looking at the allowed character ranges.
                 */
                bool any_errors = false;
                any_errors |= (sample.at(0) != '+' && sample.at(0) != '-');
                for (size_t jChar = 1; jChar <= 6; jChar++)
                {
                  any_errors |= (!is_hex_char(sample.at(jChar)));
                }
                any_errors |= (sample.at(7) != 'K' && sample.at(7) != 'J' && sample.at(7) != 'T' && sample.at(7) != 'E' && sample.at(7) != 'S');
                any_errors |= (sample.at(8) != '+' && sample.at(8) != '-');
                for (size_t jChar = 9; jChar <= 14; jChar++)
                {
                  any_errors |= (!is_hex_char(sample.at(jChar)));
                }
                any_errors |= (sample.at(15) != '_');
                for (size_t jChar = 16; jChar <= 21; jChar++)
                {
                  any_errors |= (!isdigit(sample.at(jChar)));
                }
                any_errors |= (sample.at(22) != 'R' && sample.at(22) != 'M' && sample.at(22) != 'I' && sample.at(22) != 'A' && sample.at(22) != '-' && sample.at(22) != '_');
                any_errors |= (sample.at(23) != 'R' && sample.at(23) != 'H' && sample.at(23) != '_');
                any_errors |= (sample.at(24) != '_');
                any_errors |= (sample.at(25) != 'L' && sample.at(25) != '_');
                any_errors |= (sample.at(26) != 'H' && sample.at(26) != '_');
                any_errors |= (sample.at(27) != 'L' && sample.at(27) != '_');
                any_errors |= (sample.at(28) != '_');
                any_errors |= (sample.at(29) != 'B' && sample.at(29) != '_');

                if (!any_errors)
                {
                  /**
                   * Data looks basically valid. Parse it into a struct,
                   * save in data_vec for later storage in a .xlsx file,
                   * and print the readings so the user can see what's
                   * going on.
                   */
                  parsed_sample_t parsed_sample;
                  
                  parsed_sample.T1_degC = 0.001 * (
                    static_cast<double>(parse_hex_char(sample.at(6))) + 
                    16.0 * static_cast<double>(parse_hex_char(sample.at(5))) + 
                    256.0 * static_cast<double>(parse_hex_char(sample.at(4))) +
                    4096.0 * static_cast<double>(parse_hex_char(sample.at(3))) +
                    65536.0 * static_cast<double>(parse_hex_char(sample.at(2))) +
                    1048576.0 * static_cast<double>(parse_hex_char(sample.at(1))) );

                  if (sample.at(0) == '-') parsed_sample.T1_degC *= -1.0;

                  parsed_sample.T2_degC = 0.001 * (
                    static_cast<double>(parse_hex_char(sample.at(14))) + 
                    16.0 * static_cast<double>(parse_hex_char(sample.at(13))) + 
                    256.0 * static_cast<double>(parse_hex_char(sample.at(12))) +
                    4096.0 * static_cast<double>(parse_hex_char(sample.at(11))) +
                    65536.0 * static_cast<double>(parse_hex_char(sample.at(10))) +
                    1048576.0 * static_cast<double>(parse_hex_char(sample.at(9))) );

                  if (sample.at(8) == '-') parsed_sample.T2_degC *= -1.0;

                  switch (sample.at(7))
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
                    parsed_sample.tc_type = TC_type_t::K_type;
                    break;
                  }

                  switch (sample.at(22))
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
                    parsed_sample.record_mode = Record_mode_t::Normal;
                    break;
                  }

                  switch (sample.at(23))
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
                    parsed_sample.rel_hold_mode = Rel_Hold_mode_t::Normal;
                    break;
                  }

                  if (sample.at(25) == 'L')
                  {
                    parsed_sample.limits_mode = true;
                  }
                  else
                  {
                    parsed_sample.limits_mode = false;
                  }

                  if (sample.at(29) == 'B')
                  {
                    parsed_sample.battery_low = true;
                  }
                  else
                  {
                    parsed_sample.battery_low = false;
                  }

                  parsed_sample.timestamp = line_check_queue.at(0).sample_time_s;

                  data_vec.push_back(parsed_sample);

                  if (first_sample)
                  {
                    first_sample = false;
                    T1_min_degC = parsed_sample.T1_degC;
                    T1_max_degC = parsed_sample.T1_degC;
                    T2_min_degC = parsed_sample.T2_degC;
                    T2_max_degC = parsed_sample.T2_degC;
                    double T1_T2_degC = parsed_sample.T1_degC - parsed_sample.T2_degC;
                    T1_T2_min_degC = T1_T2_degC;
                    T1_T2_max_degC = T1_T2_degC;
                  }

                  T1_min_degC = std::min(T1_min_degC, parsed_sample.T1_degC);
                  T1_max_degC = std::max(T1_max_degC, parsed_sample.T1_degC);
                  T2_min_degC = std::min(T2_min_degC, parsed_sample.T2_degC);
                  T2_max_degC = std::max(T2_max_degC, parsed_sample.T2_degC);
                  double T1_T2_degC = parsed_sample.T1_degC - parsed_sample.T2_degC;
                  T1_T2_min_degC = std::min(T1_T2_min_degC, T1_T2_degC);
                  T1_T2_max_degC = std::max(T1_T2_max_degC, T1_T2_degC);

                  std::printf("-    T1 now/min/max deg. C: %9.3f / %9.3f / %9.3f, T2 now/min/max deg. C: %9.3f / %9.3f / %9.3f\n", parsed_sample.T1_degC, T1_min_degC, T1_max_degC, parsed_sample.T2_degC, T2_min_degC, T2_max_degC);
                  std::printf("  T1-T2 now/min/max deg. C: %9.3f / %9.3f / %9.3f, TC Type: %c, Batt: %s, Time: %.3fs\n\n", T1_T2_degC, T1_T2_min_degC, T1_T2_max_degC, sample.at(7), parsed_sample.battery_low ? "low" : "ok", parsed_sample.timestamp);

                  /**
                   * The thermocouple reader is more likely to stay on for long periods
                   * of time if it's in Record mode.
                   * This bit of code attempts to keep the reader in Record MAX mode.
                   * There is a minimum 100ms wait so that we don't spam the reader
                   * with commands.
                   */
                  if (std::chrono::milliseconds(100) < (present_time - last_send_time) &&
                      parsed_sample.record_mode != Record_mode_t::Max)
                  {
                    last_send_time = present_time;

                    strncpy(write_buffer, "G\r\n", write_buf_size);
                    ncomwritten = 0u;
                    if (!WriteFile(tc_reader_com_port_handle, write_buffer, static_cast<DWORD>(write_buf_size-1u), &ncomwritten, NULL))
                    {
                      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
                      exit_cleanup();
                      return EXIT_FAILURE;
                    }

                    if (ncomwritten != static_cast<DWORD>(write_buf_size-1u))
                    {
                      std::printf("Error writing to TC Reader COM port, %s.\n\n", tc_reader_com_port_printable.c_str());
                      exit_cleanup();
                      return EXIT_FAILURE;
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
          }

          /**
           * Erase the data that had the wrong length before
           * the \r\n.
           */
          if (read_data.size() > line_end_pos + 2)
          {
            read_data = read_data.substr(line_end_pos+2);
          }
          else
          {
            read_data.clear();
          }
        }
      }
    }

    /**
     * Waiting 10ms between attempts at reading serial port
     * so as not to consume too many resources.
     * This is plenty fast for 32 bytes 3 times per second
     * at 1200 baud.
     */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
