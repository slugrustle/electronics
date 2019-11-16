/**
 * parse_config_file.cpp
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

#include <algorithm>
#include <cstdio>
#include "parser_helpers.h"
#include "parse_config_file.h"

/**
 * If the input string is a case insensitive match for a ZenerSocket
 * enum value, socket is set to that value, and true is returned.
 * Otherwise, socket is not modified, and false is returned.
 */
bool map_zener_string_enum(const std::string input, ZenerSocket &socket)
{
  if (case_insensitive_same(input, "J5L")) { socket = ZenerSocket::J5L; return true; }
  if (case_insensitive_same(input, "J5R")) { socket = ZenerSocket::J5R; return true; }
  if (case_insensitive_same(input, "J6L")) { socket = ZenerSocket::J6L; return true; }
  if (case_insensitive_same(input, "J6R")) { socket = ZenerSocket::J6R; return true; }
  if (case_insensitive_same(input, "J7L")) { socket = ZenerSocket::J7L; return true; }
  if (case_insensitive_same(input, "J7R")) { socket = ZenerSocket::J7R; return true; }
  if (case_insensitive_same(input, "J8L")) { socket = ZenerSocket::J8L; return true; }
  if (case_insensitive_same(input, "J8R")) { socket = ZenerSocket::J8R; return true; }
  if (case_insensitive_same(input, "J9L")) { socket = ZenerSocket::J9L; return true; }
  if (case_insensitive_same(input, "J9R")) { socket = ZenerSocket::J9R; return true; }
  return false;
}

test_config_t parse_config_file(std::ifstream &input_stream)
{
  test_config_t test_config = { 0 };
  test_config.read_ok = false;
  test_config.diolan_serial_present = false;
  
  test_config.num_gpio_required = 0u;
  for (size_t jZener = 0u; jZener < NUM_ZENER_SOCKETS; jZener++)
  {
    uint32_t this_num_gpio_required = static_cast<uint32_t>(ZENER_RELAY_PIN.at(jZener)) + 1u;
    test_config.num_gpio_required = std::max(this_num_gpio_required, test_config.num_gpio_required);
  }
  for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
  {
    uint32_t this_num_gpio_required = static_cast<uint32_t>(RESISTOR_RELAY_PIN.at(jResistor)) + 1u;
    test_config.num_gpio_required = std::max(this_num_gpio_required, test_config.num_gpio_required);
  }

  bool any_read_errors = false;
  std::array<bool, NUM_SERIAL_DEVICES> found_com_ports;
  std::fill(found_com_ports.begin(), found_com_ports.end(), false);
  std::array<bool, NUM_SERIAL_DEVICES> found_baud_rates;
  std::fill(found_baud_rates.begin(), found_baud_rates.end(), false);
  std::array<bool, NUM_SERIAL_DEVICES> found_parities;
  std::fill(found_parities.begin(), found_parities.end(), false);
  bool found_zeners_installed = false;
  std::array<bool, NUM_ZENER_SOCKETS> found_voltage_steps;
  std::fill(found_voltage_steps.begin(), found_voltage_steps.end(), false);
  std::array<bool, NUM_ZENER_SOCKETS> found_zener_currents;
  std::fill(found_zener_currents.begin(), found_zener_currents.end(), false);
  std::array<bool, NUM_ZENER_SOCKETS> found_zener_powers;
  std::fill(found_zener_powers.begin(), found_zener_powers.end(), false);
  std::array<bool, NUM_FIXTURE_RESISTORS> found_resistor_powers;
  std::fill(found_resistor_powers.begin(), found_resistor_powers.end(), false);

  std::string line;
  size_t jLine = 0u;
  const std::set<char> token_delimeters = {',', ' ', '\n', '\r', '\t', '\f', '\v'};
  while (std::getline(input_stream, line))
  {
    jLine++;
    std::vector<std::string> tokens = tokenize(line, token_delimeters);
    if (!tokens.empty() && tokens.front().at(0) != '#')
    {
      /* Non-empty non-comment lines are parsed in this block. */
      std::string &first_token = tokens.front();

      bool found_any_com_port = false;
      for (size_t jDevice = 0u; jDevice < NUM_SERIAL_DEVICES; jDevice++)
      {
        std::string field_name = SERIAL_DEVICE_NAMES.at(jDevice) + "_COM_PORT";

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_com_ports.at(jDevice))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_com_ports.at(jDevice) = true;
            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              if (!case_insensitive_same(second_token.substr(0u, 3u), "COM"))
              {
                std::printf("Error: first argument to %s does not start with COM on line %zu of configuration file.\n", field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else
              {
                bool parse_ok = false;
                int64_t port_number = parse_int64(second_token.substr(3u), parse_ok);
                if (!parse_ok || port_number < 1ll || port_number > 255ll)
                {
                  std::printf("Error: %s argument has non-integer or <1 or >255 for COM port number on line %zu of configuration file.\n", field_name.c_str(), jLine);
                  any_read_errors = true;
                }
                else
                {
                  test_config.com_port_printables.at(jDevice) = to_uppercase(second_token);

                  if (port_number <= 9ll)
                  {
                    test_config.com_ports.at(jDevice) = test_config.com_port_printables.at(jDevice);
                  }
                  else
                  {
                    test_config.com_ports.at(jDevice) = "\\\\.\\" + test_config.com_port_printables.at(jDevice);
                  }
                }
              }
            }
          }

          found_any_com_port = true;
          break;
        }
      }
      if (found_any_com_port) continue;

      bool found_any_baud_rate = false;
      for (size_t jDevice = 0u; jDevice < NUM_SERIAL_DEVICES; jDevice++)
      {
        std::string field_name = SERIAL_DEVICE_NAMES.at(jDevice) + "_BAUD_RATE";

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_baud_rates.at(jDevice))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_baud_rates.at(jDevice) = true;
            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              bool parse_ok = false;
              int64_t baud_rate = parse_int64(second_token, parse_ok);

              if (!parse_ok)
              {
                std::printf("Error: non-integer value for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else if (baud_rate == static_cast<int64_t>(BaudRate::B4800)) test_config.baud_rates.at(jDevice) = BaudRate::B4800;
              else if (baud_rate == static_cast<int64_t>(BaudRate::B9600)) test_config.baud_rates.at(jDevice) = BaudRate::B9600;
              else if (baud_rate == static_cast<int64_t>(BaudRate::B19200)) test_config.baud_rates.at(jDevice) = BaudRate::B19200;
              else if (baud_rate == static_cast<int64_t>(BaudRate::B38400)) test_config.baud_rates.at(jDevice) = BaudRate::B38400;
              else if (baud_rate == static_cast<int64_t>(BaudRate::B57600)) test_config.baud_rates.at(jDevice) = BaudRate::B57600;
              else if (baud_rate == static_cast<int64_t>(BaudRate::B115200)) test_config.baud_rates.at(jDevice) = BaudRate::B115200;
              else
              {
                std::printf("Error: unsupported value for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
                any_read_errors = true;
              }
            }
          }

          found_any_baud_rate = true;
          break;
        }
      }
      if (found_any_baud_rate) continue;

      bool found_any_parity = false;
      for (size_t jDevice = 0u; jDevice < NUM_SERIAL_DEVICES; jDevice++)
      {
        std::string field_name = SERIAL_DEVICE_NAMES.at(jDevice) + "_PARITY";

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_parities.at(jDevice))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_parities.at(jDevice) = true;
            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              if (case_insensitive_same(second_token, "EVEN")) test_config.parities.at(jDevice) = ParityType::EVEN;
              else if (case_insensitive_same(second_token, "ODD")) test_config.parities.at(jDevice) = ParityType::ODD;
              else if (case_insensitive_same(second_token, "NONE")) test_config.parities.at(jDevice) = ParityType::NONE;
              else
              {
                std::printf("Error: invalid argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
                any_read_errors = true;
              }
            }
          }
          
          found_any_parity = true;
          break;
        }
      }
      if (found_any_parity) continue;

      if (case_insensitive_same(first_token, "DIOLAN_SERIAL_NUMBER"))
      {
        if (test_config.diolan_serial_present)
        {
          std::printf("Warning: redefinition of DIOLAN_SERIAL_NUMBER on line %zu of configuration file.\n", jLine);
        }
        else if (tokens.size() > 1u)
        {
          test_config.diolan_serial_present = true;

          if (tokens.size() > 2u)
          {
            std::printf("Warning: more than one argument to DIOLAN_SERIAL_NUMBER on line %zu of configuration file.\n", jLine);
          }

          std::string &second_token = tokens.at(1);
          bool parse_ok = false;
          int64_t diolan_serial = parse_int64(second_token, parse_ok);

          if (!parse_ok || diolan_serial < 0ll || diolan_serial > static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))
          {
            std::printf("Error: non-integer or out of range value for DIOLAN_SERIAL_NUMBER on line %zu of configuration file.\n", jLine);
            any_read_errors = true;
          }
          else
          {
            test_config.diolan_serial_number = static_cast<uint32_t>(diolan_serial);
          }
        }

        continue;
      }

      if (case_insensitive_same(first_token, "ZENERS_INSTALLED"))
      {
        if (found_zeners_installed)
        {
          std::printf("Warning: redefinition of ZENERS_INSTALLED on line %zu of configuration file.\n", jLine);
        }
        else
        {
          found_zeners_installed = true;
          if (tokens.size() < 2u)
          {
            std::printf("Error: no argument for ZENERS_INSTALLED on line %zu of configuration file.\n", jLine);
            any_read_errors = true;
          }
          else
          {
            if (tokens.size() > NUM_ZENER_SOCKETS + 1u)
            {
              std::printf("Warning: more than %zu arguments to ZENERS_INSTALLED on line %zu of configuration file.\n", NUM_ZENER_SOCKETS, jLine);
            }

            for (size_t jToken = 1u; jToken < std::min(static_cast<size_t>(NUM_ZENER_SOCKETS + 1u), tokens.size()); jToken++)
            {
              ZenerSocket socket;
              if (!map_zener_string_enum(tokens.at(jToken), socket))
              {
                std::printf("Error: invalid argument for ZENERS_INSTALLED on line %zu of configuration file.\n", jLine);
                any_read_errors = true;
              }
              else
              {
                std::pair<std::set<ZenerSocket>::iterator, bool> retval = test_config.zeners_installed.insert(socket);
                if (!retval.second)
                {
                  std::printf("Warning: duplicate argument to ZENERS_INSTALLED on line %zu of configuration file.\n", jLine);
                }
              }
            }
          }
        }

        continue;
      }

      bool found_any_voltage_step = false;
      for (size_t jSocket = 0u; jSocket < NUM_ZENER_SOCKETS; jSocket++)
      {
        std::string field_name = "VOLTAGE_STEP_" + ZENER_SOCKET_NAMES.at(jSocket);

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_voltage_steps.at(jSocket))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_voltage_steps.at(jSocket) = true;

            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              double voltage_step_val = parse_double(second_token);
              if (std::isnan(voltage_step_val) || voltage_step_val < VOLTAGE_STEP_LOWER_LIMIT || voltage_step_val > VOLTAGE_STEP_UPPER_LIMIT)
              {
                std::printf("Error: non-number or <%.1e V or >%.1e V for %s on line %zu of configuration file.\n", VOLTAGE_STEP_LOWER_LIMIT, VOLTAGE_STEP_UPPER_LIMIT, field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else
              {
                test_config.voltage_steps.at(jSocket) = voltage_step_val;
              }
            }
          }

          found_any_voltage_step = true;
          break;
        }
      }
      if (found_any_voltage_step) continue;

      bool found_any_max_zener_current = false;
      for (size_t jSocket = 0u; jSocket < NUM_ZENER_SOCKETS; jSocket++)
      {
        std::string field_name = "MAX_CURRENT_" + ZENER_SOCKET_NAMES.at(jSocket);

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_zener_currents.at(jSocket))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_zener_currents.at(jSocket) = true;

            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              double current_val = parse_double(second_token);
              if (std::isnan(current_val) || current_val < MAX_CURRENT_LOWER_LIMIT || current_val > MAX_CURRENT_UPPER_LIMIT)
              {
                std::printf("Error: non-number or <%.1e A or >%.1e A for %s on line %zu of configuration file.\n", MAX_CURRENT_LOWER_LIMIT, MAX_CURRENT_UPPER_LIMIT, field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else
              {
                test_config.max_zener_currents.at(jSocket) = current_val;
              }
            }
          }

          found_any_max_zener_current = true;
          break;
        }
      }
      if (found_any_max_zener_current) continue;

      bool found_any_max_zener_power = false;
      for (size_t jSocket = 0u; jSocket < NUM_ZENER_SOCKETS; jSocket++)
      {
        std::string field_name = "MAX_POWER_" + ZENER_SOCKET_NAMES.at(jSocket);

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_zener_powers.at(jSocket))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_zener_powers.at(jSocket) = true;

            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              double power_val = parse_double(second_token);
              if (std::isnan(power_val) || power_val < MAX_POWER_LOWER_LIMIT || power_val > MAX_POWER_UPPER_LIMIT)
              {
                std::printf("Error: non-number or <%.1e W or >%.1e W for %s on line %zu of configuration file.\n", MAX_POWER_LOWER_LIMIT, MAX_POWER_UPPER_LIMIT, field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else
              {
                test_config.max_zener_powers.at(jSocket) = power_val;
              }
            }
          }

          found_any_max_zener_power = true;
          break;
        }
      }
      if (found_any_max_zener_power) continue;

      bool found_any_max_resistor_power = false;
      for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
      {
        std::string field_name = "MAX_POWER_" + FIXTURE_RESISTOR_NAMES.at(jResistor);

        if (case_insensitive_same(first_token, field_name))
        {
          if (found_resistor_powers.at(jResistor))
          {
            std::printf("Warning: redefinition of %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
          }
          else
          {
            found_resistor_powers.at(jResistor) = true;

            if (tokens.size() < 2u)
            {
              std::printf("Error: no argument for %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              any_read_errors = true;
            }
            else
            {
              if (tokens.size() > 2u)
              {
                std::printf("Warning: more than one argument to %s on line %zu of configuration file.\n", field_name.c_str(), jLine);
              }

              std::string &second_token = tokens.at(1);
              double power_val = parse_double(second_token);
              if (std::isnan(power_val) || power_val < MAX_POWER_LOWER_LIMIT || power_val > MAX_POWER_UPPER_LIMIT)
              {
                std::printf("Error: non-number or <%.1e W or >%.1e W for %s on line %zu of configuration file.\n", MAX_POWER_LOWER_LIMIT, MAX_POWER_UPPER_LIMIT, field_name.c_str(), jLine);
                any_read_errors = true;
              }
              else
              {
                test_config.max_resistor_powers.at(jResistor) = power_val;
              }
            }
          }

          found_any_max_resistor_power = true;
          break;
        }
      }
      if (found_any_max_resistor_power) continue;

      std::printf("Warning: Unrecognized non-comment on line %zu of configuration file.\n", jLine);
    }
  }

  for (size_t jDevice1 = 0u; jDevice1 < NUM_SERIAL_DEVICES - 1u; jDevice1++)
  {
    for (size_t jDevice2 = jDevice1 + 1u; jDevice2 < NUM_SERIAL_DEVICES; jDevice2++)
    {
      if (found_com_ports.at(jDevice1) &&
          found_com_ports.at(jDevice2) &&
          test_config.com_ports.at(jDevice1).compare(test_config.com_ports.at(jDevice2)) == 0)
      {
        std::printf("Error: The %s and %s were configured for the same COM port. This is not allowed.\n",
                    SERIAL_DEVICE_NAMES.at(jDevice1).c_str(), SERIAL_DEVICE_NAMES.at(jDevice2).c_str());
        any_read_errors = true;
      }
    }
  }

  bool something_missing = false;
  for (size_t jDevice = 0u; jDevice < NUM_SERIAL_DEVICES; jDevice++)
  {
    if (!found_com_ports.at(jDevice))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing %s_COM_PORT definition.\n", SERIAL_DEVICE_NAMES.at(jDevice).c_str());
    }
    if (!found_baud_rates.at(jDevice))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing %s_BAUD_RATE definition.\n", SERIAL_DEVICE_NAMES.at(jDevice).c_str());
    }
    if (!found_parities.at(jDevice))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing %s_PARITY definition.\n", SERIAL_DEVICE_NAMES.at(jDevice).c_str());
    }
  }

  if (!found_zeners_installed)
  {
    something_missing = true;
    std::printf("Error: configuration file is missing ZENERS_INSTALLED definition.\n");
  }

  for (size_t jSocket = 0u; jSocket < NUM_ZENER_SOCKETS; jSocket++)
  {
    if (!found_voltage_steps.at(jSocket))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing VOLTAGE_STEP_%s definition.\n", ZENER_SOCKET_NAMES.at(jSocket).c_str());
    }
    if (!found_zener_currents.at(jSocket))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing MAX_CURRENT_%s definition.\n", ZENER_SOCKET_NAMES.at(jSocket).c_str());
    }
    if (!found_zener_powers.at(jSocket))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing MAX_POWER_%s definition.\n", ZENER_SOCKET_NAMES.at(jSocket).c_str());
    }
  }

  for (size_t jResistor = 0u; jResistor < NUM_FIXTURE_RESISTORS; jResistor++)
  {
    if (!found_resistor_powers.at(jResistor))
    {
      something_missing = true;
      std::printf("Error: configuration file is missing MAX_POWER_%s definition.\n", FIXTURE_RESISTOR_NAMES.at(jResistor).c_str());
    }
  }

  test_config.read_ok = (!something_missing && !any_read_errors);

  return test_config;
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
