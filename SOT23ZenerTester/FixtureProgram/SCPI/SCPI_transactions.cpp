/**
 * SCPI_transactions.cpp
 * 
 * Definitions of functions that manage more complex SCPI transactions to 
 * configure or otherwise interact with test equipment.
 * 
 * Written in 2019 by Ben Tesch.
 * Originally distributed at https://github.com/slugrustle/electronics
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * The text of the CC0 Public Domain Dedication should be reproduced at the
 * end of this file. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include "SCPI_transactions.h"
#include "SCPI_helpers.h"
#include "parser_helpers.h"

/**
 * Checks whether device connected to PSU COM port replies to the
 * SCPI *IDN? query in a manner consistent with an Itech IT6100B
 * series power supply.
 *
 * Also returns any retrieved IDN response in id_string.
 *
 * Other power supplies might require tweaked commands to work
 * with the Zener test fixture.
 */
Check_ID_status check_PSU_ID(HANDLE com_port_handle, std::string &id_string)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*IDN?");
  if (SCPIret != SCPIstatus::ALL_OK) return Check_ID_status::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return Check_ID_status::COM_ERROR;

  id_string.clear();
  id_string = trim_whitespace(scpi_read);

  if (scpi_read.empty()) return Check_ID_status::ZERO_DATA;

  const std::set<char> delimeters = {',', ' ', '\t', '\r', '\n', '\f', '\v'};
  std::vector<std::string> tokens = tokenize(scpi_read, delimeters);

  if (tokens.size() < 2u) return Check_ID_status::ID_MISMATCH;
  if (!case_insensitive_same(tokens.at(0), "Itech")) return Check_ID_status::ID_MISMATCH;
  if (tokens.at(1u).size() != 7u || 
      !case_insensitive_same(tokens.at(1u).substr(0u, 4u), "IT61") ||
      !std::isdigit(tokens.at(1u).at(4u)) ||
      !std::isdigit(tokens.at(1u).at(5u)) ||
      std::toupper(tokens.at(1u).at(6u)) != 'B') return Check_ID_status::ID_MISMATCH;
  
  return Check_ID_status::ID_MATCH;
}

/**
* Checks whether device connected to DMM COM port replies to the
* SCPI *IDN? query in a manner consistent with a Rigol DM3068
* DMM.
*
* Also returns any retrieved IDN response in id_string.
*
* Other multimeters might require tweaked commands to work
* with the Zener test fixture.
*/
Check_ID_status check_DMM_ID(HANDLE com_port_handle, std::string &id_string)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*IDN?");
  if (SCPIret != SCPIstatus::ALL_OK) return Check_ID_status::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return Check_ID_status::COM_ERROR;

  id_string.clear();
  id_string = trim_whitespace(scpi_read);

  if (scpi_read.empty()) return Check_ID_status::ZERO_DATA;

  const std::set<char> delimeters = {',', '\t', '\r', '\n', '\f', '\v'};
  std::vector<std::string> tokens = tokenize(scpi_read, delimeters);

  if (tokens.size() < 2u) return Check_ID_status::ID_MISMATCH;
  if (!case_insensitive_same(tokens.at(0), "Rigol Technologies")) return Check_ID_status::ID_MISMATCH;
  if (!case_insensitive_same(tokens.at(1), "DM3068")) return Check_ID_status::ID_MISMATCH;

  return Check_ID_status::ID_MATCH;
}

/**
 * Checks whether the power-on status clear flag is 1.
 * Attempts to set this to 1 if it is 0.
 */
PSC_status set_PSC(HANDLE com_port_handle)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*PSC?");
  if (SCPIret != SCPIstatus::ALL_OK) return PSC_status::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return PSC_status::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return PSC_status::ZERO_DATA;
  if (scpi_read.at(0) == '1') return PSC_status::PSC_ON;
  if (scpi_read.at(0) == '0')
  {
    /* Attempt to set PSC to 1. */
    SCPIret = SCPIwrite(com_port_handle, "*PSC 1");
    if (SCPIret != SCPIstatus::ALL_OK) return PSC_status::COM_ERROR;

    SCPIret = SCPIread(com_port_handle, scpi_read);
    if (SCPIret != SCPIstatus::ALL_OK) return PSC_status::COM_ERROR;

    scpi_read = trim_whitespace(scpi_read);
    if (scpi_read.empty()) return PSC_status::ZERO_DATA;
    if (scpi_read.at(0) == '1') return PSC_status::PSC_ON;
    if (scpi_read.at(0) == '0') return PSC_status::PSC_OFF;
  }

  return PSC_status::UNRECOGNIZED_RESPONSE;
}

/**
 * Runs the sequence
 * *RST     reset
 * *CLS     clear status registers
 * *ESE 0   clear standard event status enable register
 * *ESE?    check that standard event status enable register is cleared
 * *SRE 0   clear service request enable register
 * *SRE?    check that service request enable register is cleared
 */
KnownStateStatus set_known_PSU_state(HANDLE com_port_handle)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*RST");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*CLS");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*ESE 0");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*ESE?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t ese_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (ese_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, "*SRE 0");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*SRE?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  bool sre_clear = false;
  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t sre_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (sre_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  return KnownStateStatus::ALL_OK;
}

/**
 * Cannot use *RST or :SYStem:CONFigure:DEFault because that would reset
 * the baud rate. DM3068 also has a bug where *RST does not reset the
 * trigger from single to auto, even though the registers read that way.
 *
 * Instead, this executes the following:
 * *CLS     clear status registers
 * *ESE 0   clear standard event status enable register
 * *ESE?    check that standard event status enable register is cleared
 * *SRE 0   clear service request enable register
 * *SRE?    check that service request enable register is cleared
 * :STAT:PRES clear operation status enable and questionable status enable registers
 * :STAT:OPER:ENAB? check that operation status enable register is clear
 * :STAT:QUES:ENAB? check that questionable status enable register is clear
 * :CALC:FUNC NONE turn off stats calculations
 * :CALC:FUNC? check that stats calculations are off
 * :FUNC:VOLT:DC set for dc voltage measurement
 * :FUNC? check we are measuring dc voltage
 * if autorange is true,
 *   :MEAS AUTO sets measurements for autorange. no way to check this
 * otherwise,
 *   :MEAS MANU configures manual measurement range determination. no way to check this
 *   :MEAS:VOLT:DC 2 sets 20V measurement range
 *   :MEAS:VOLT:DC:RANGE? checks this
 * :MEAS:VOLT:DC:IMPE 10G set 10 Gigohm measurement impedance for ranges up through 20V
 * :MEAS:VOLT:DC:IMPE? check 10G measurement impedance
 * :TRIG:SOUR SINGLE set for single trigger
 * :TRIG:SOUR? check single trigger
 */
KnownStateStatus set_known_DMM_state(HANDLE com_port_handle, const bool autorange)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*CLS");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*ESE 0");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*ESE?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t ese_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (ese_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, "*SRE 0");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "*SRE?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t sre_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (sre_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, ":STAT:PRES");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, ":STAT:OPER:ENAB?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t ose_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (ose_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, ":STAT:QUES:ENAB?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else
  {
    bool parse_ok = false;
    int64_t qse_value = parse_int64(scpi_read, parse_ok);
    if (!parse_ok) return KnownStateStatus::UNRECOGNIZED_RESPONSE;
    else if (qse_value != 0ll) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, ":CALC:FUNC NONE");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, ":CALC:FUNC?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else if (!case_insensitive_same(scpi_read, "NONE")) return KnownStateStatus::SETTING_CHECK_FAILED;

  SCPIret = SCPIwrite(com_port_handle, ":FUNC:VOLT:DC");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  SCPIret = SCPIwrite(com_port_handle, ":FUNC?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else if (!case_insensitive_same(scpi_read, "DCV")) return KnownStateStatus::SETTING_CHECK_FAILED;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  if (autorange)
  {
    SCPIret = SCPIwrite(com_port_handle, ":MEAS AUTO");
    if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  else
  {
    SCPIret = SCPIwrite(com_port_handle, ":MEAS MANU");
    if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    SCPIret = SCPIwrite(com_port_handle, ":MEAS:VOLT:DC 2");
    if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    SCPIret = SCPIwrite(com_port_handle, ":MEAS:VOLT:DC:RANGE?");
    if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

    SCPIret = SCPIread(com_port_handle, scpi_read);
    if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

    scpi_read = trim_whitespace(scpi_read);
    if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
    else if (!case_insensitive_same(scpi_read, "2")) return KnownStateStatus::SETTING_CHECK_FAILED;
  }

  SCPIret = SCPIwrite(com_port_handle, ":MEAS:VOLT:DC:IMPE 10G");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  SCPIret = SCPIwrite(com_port_handle, ":MEAS:VOLT:DC:IMPE?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else if (!case_insensitive_same(scpi_read, "10G")) return KnownStateStatus::SETTING_CHECK_FAILED;

  SCPIret = SCPIwrite(com_port_handle, ":TRIG:SOUR SINGLE");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  SCPIret = SCPIwrite(com_port_handle, ":TRIG:SOUR?");
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return KnownStateStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return KnownStateStatus::ZERO_DATA;
  else if (!case_insensitive_same(scpi_read, "SINGLE")) return KnownStateStatus::SETTING_CHECK_FAILED;

  return KnownStateStatus::ALL_OK;
}

/**
 * Checks the result of *OPC?. Useful to make sure previous
 * commands have completed.
 *
 * There are so many retries because the reply to *OPC?
 * takes a human-noticeable length of time in an interactive
 * terminal session.
 */
OPC_status check_OPC(HANDLE com_port_handle)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "*OPC?");
  if (SCPIret != SCPIstatus::ALL_OK) return OPC_status::COM_ERROR;

  for (uint8_t jRetry = 0u; jRetry < 5u; jRetry++)
  {
    if (jRetry == 0u) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    else std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string scpi_read;
    SCPIret = SCPIread(com_port_handle, scpi_read);
    if (SCPIret != SCPIstatus::ALL_OK) return OPC_status::COM_ERROR;

    scpi_read = trim_whitespace(scpi_read);
    if (!scpi_read.empty())
    {
      if (scpi_read.at(0) == '1') return OPC_status::OPC_TRUE;
      if (scpi_read.at(0) == '0') return OPC_status::OPC_FALSE;
      return OPC_status::UNRECOGNIZED_RESPONSE;
    }
  }

  return OPC_status::ZERO_DATA;
}

/**
 * Attempts to find min/max voltage, voltage protection, and
 * current levels. Also checks if voltage protection is on.
 */
GetDataStatus get_PSU_limits(HANDLE com_port_handle, PSU_limits_t &limits)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:LEV? MIN");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.voltage_out_min = parse_double(scpi_read);
  if (std::isnan(limits.voltage_out_min)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:LEV? MAX");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.voltage_out_max = parse_double(scpi_read);
  if (std::isnan(limits.voltage_out_max)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:LEV? MIN");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.voltage_protect_min = parse_double(scpi_read);
  if (std::isnan(limits.voltage_protect_min)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:LEV? MAX");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.voltage_protect_max = parse_double(scpi_read);
  if (std::isnan(limits.voltage_protect_max)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:STAT?");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  if (scpi_read.at(0) == '0') limits.voltage_protect_on = false;
  else if (scpi_read.at(0) == '1') limits.voltage_protect_on = true;
  else return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:CURR:LEV? MIN");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.current_out_min = parse_double(scpi_read);
  if (std::isnan(limits.current_out_min)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  SCPIret = SCPIwrite(com_port_handle, "SOUR:CURR:LEV? MAX");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  limits.current_out_max = parse_double(scpi_read);
  if (std::isnan(limits.current_out_max)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  return GetDataStatus::ALL_OK;
}

GetDataStatus meas_PSU_voltage(HANDLE com_port_handle, double &voltage)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "MEAS:VOLT?");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  voltage = parse_double(scpi_read);
  if (std::isnan(voltage)) return GetDataStatus::UNRECOGNIZED_RESPONSE;
  
  return GetDataStatus::ALL_OK;
}

GetDataStatus meas_PSU_DVM(HANDLE com_port_handle, double &voltage)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "MEAS:DVM?");
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return GetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return GetDataStatus::ZERO_DATA;
  voltage = parse_double(scpi_read);
  if (std::isnan(voltage)) return GetDataStatus::UNRECOGNIZED_RESPONSE;

  return GetDataStatus::ALL_OK;
}

GetDataStatus meas_DMM_DCV(HANDLE com_port_handle, double &voltage)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, ":MEAS:VOLT:DC?");
  if (SCPIret != SCPIstatus::ALL_OK)
  {
    std::printf("meas_DMM_DCV scpi write com error.\n");
    return GetDataStatus::COM_ERROR;
  }

  /**
   * 10NPLC integration time plus 5% is 175ms at 60Hz and 210ms at 50Hz.
   * Multiply 210ms by 4 for 4 voltage ranges, getting 840ms.
   * Check every 100ms plus one beyond 840ms for 9 attempts.
   * All the delays in SCPIread make this much longer anyway.
   */
  std::string scpi_read;

  for (uint8_t jAttempt = 0u; jAttempt < 9u; jAttempt++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
    SCPIret = SCPIread(com_port_handle, scpi_read);
    if (SCPIret != SCPIstatus::ALL_OK && SCPIret != SCPIstatus::ZERO_DATA)
    {
      std::printf("meas_DMM_DCV scpi read attempt %u com error.\n", jAttempt + 1u);
      return GetDataStatus::COM_ERROR;
    }

    scpi_read = trim_whitespace(scpi_read);
    if (!scpi_read.empty()) break;
  }

  if (scpi_read.empty())
  {
    std::printf("meas_DMM_DCV scpi read zero data.\n");
    return GetDataStatus::ZERO_DATA;
  }
  voltage = parse_double(scpi_read);
  if (std::isnan(voltage))
  {
    std::printf("meas_DMM_DCV scpi data parse NaN.\n");
    return GetDataStatus::UNRECOGNIZED_RESPONSE;
  }

  return GetDataStatus::ALL_OK;
}

/**
 * Sets output voltage protection to the voltage voltage_protection_level,
 * checks that setting, then turns on voltage protection and checks that also.
 */
SetDataStatus set_PSU_protection(HANDLE com_port_handle, double voltage_protection_level)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:LEV %.3f", voltage_protection_level);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:LEV?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  double volt_prot_read = parse_double(scpi_read);
  if (std::isnan(volt_prot_read)) return SetDataStatus::UNRECOGNIZED_RESPONSE;
  if (std::fabs(volt_prot_read - 0.001 * std::round(1000.0 * voltage_protection_level)) > 1.001e-3)
    return SetDataStatus::SETTING_MISMATCH;
  
  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:STAT ON");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:PROT:STAT?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  if (scpi_read.at(0) == '0') return SetDataStatus::SETTING_MISMATCH;
  else if (scpi_read.at(0) == '1') return SetDataStatus::ALL_OK;
  else return SetDataStatus::UNRECOGNIZED_RESPONSE;
}

SetDataStatus set_PSU_current(HANDLE com_port_handle, double current_level)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "SOUR:CURR:LEV %.3f", current_level);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SOUR:CURR:LEV?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  double current_level_read = parse_double(scpi_read);
  if (std::isnan(current_level_read)) return SetDataStatus::UNRECOGNIZED_RESPONSE;
  if (std::fabs(current_level_read - 0.001 * std::round(1000.0 * current_level)) > 1.001e-3)
    return SetDataStatus::SETTING_MISMATCH;
  
  return SetDataStatus::ALL_OK;
}

SetDataStatus set_PSU_voltage(HANDLE com_port_handle, double voltage_level)
{
  SCPIstatus SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:LEV %.3f", voltage_level);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SOUR:VOLT:LEV?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  double voltage_level_read = parse_double(scpi_read);
  if (std::isnan(voltage_level_read)) return SetDataStatus::UNRECOGNIZED_RESPONSE;
  if (std::fabs(voltage_level_read - 0.001 * std::round(1000.0 * voltage_level)) > 1.001e-3)
    return SetDataStatus::SETTING_MISMATCH;

  return SetDataStatus::ALL_OK;
}

SetDataStatus set_PSU_output(HANDLE com_port_handle, bool output_on)
{
  SCPIstatus SCPIret;
  if (output_on) SCPIret = SCPIwrite(com_port_handle, "SOUR:OUTP:STAT ON");
  else SCPIret = SCPIwrite(com_port_handle, "SOUR:OUTP:STAT OFF");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SOUR:OUTP:STAT?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  if (scpi_read.at(0) == '0')
  {
    if (output_on) return SetDataStatus::SETTING_MISMATCH;
    else return SetDataStatus::ALL_OK;
  }
  else if (scpi_read.at(0) == '1')
  {
    if (output_on) return SetDataStatus::ALL_OK;
    else return SetDataStatus::SETTING_MISMATCH;
  }
  else return SetDataStatus::UNRECOGNIZED_RESPONSE;
}

SetDataStatus set_PSU_sense(HANDLE com_port_handle, bool sense_remote)
{
  SCPIstatus SCPIret;
  if (sense_remote) SCPIret = SCPIwrite(com_port_handle, "SYST:SENS ON");
  else SCPIret = SCPIwrite(com_port_handle, "SYST:SENS OFF");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  SCPIret = SCPIwrite(com_port_handle, "SYST:SENS?");
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  std::string scpi_read;
  SCPIret = SCPIread(com_port_handle, scpi_read);
  if (SCPIret != SCPIstatus::ALL_OK) return SetDataStatus::COM_ERROR;

  scpi_read = trim_whitespace(scpi_read);
  if (scpi_read.empty()) return SetDataStatus::ZERO_DATA;
  if (scpi_read.at(0) == '0')
  {
    if (sense_remote) return SetDataStatus::SETTING_MISMATCH;
    else return SetDataStatus::ALL_OK;
  }
  else if (scpi_read.at(0) == '1')
  {
    if (sense_remote) return SetDataStatus::ALL_OK;
    else return SetDataStatus::SETTING_MISMATCH;
  }
  else return SetDataStatus::UNRECOGNIZED_RESPONSE;
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
