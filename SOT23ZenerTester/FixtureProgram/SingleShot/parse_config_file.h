/**
 * parse_config_file.h
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

#ifndef PARSE_CONFIG_FILE_H_
#define PARSE_CONFIG_FILE_H_

#include <cinttypes>
#include <array>
#include <set>
#include <string>
#include <fstream>
#define NOMINMAX
#include <Windows.h>
#include <WinBase.h>

/**
 * Constants.
 */
const size_t NUM_SERIAL_DEVICES = 2u;
const size_t NUM_ZENER_SOCKETS = 10u;
const size_t NUM_FIXTURE_RESISTORS = 5u;
const double TEST_VOLTAGE_LOWER_LIMIT = 1.0e-3;
const double TEST_VOLTAGE_UPPER_LIMIT = 1.0e3;

/**
* Pin number definitions come from J5 pinout for Diolan DLN-4M
* and from Rev 1 of SOT23ZenerTester Schematic / PCB.
*/
const std::array<uint16_t, NUM_ZENER_SOCKETS> ZENER_RELAY_PIN = {5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u};
const std::array<uint16_t, NUM_FIXTURE_RESISTORS> RESISTOR_RELAY_PIN = {0u, 1u, 2u, 3u, 4u};

const std::array<double, NUM_FIXTURE_RESISTORS> NOMINAL_RESISTANCES = {100.0, 1.0e3, 10.0e3, 100.0e3, 1.0e6};
const std::array<std::string, NUM_FIXTURE_RESISTORS> FIXTURE_RESISTOR_NAMES = {"100R", "1K", "10K", "100K", "1M"};
const std::array<std::string, NUM_ZENER_SOCKETS> ZENER_SOCKET_NAMES = {"J5L", "J5R", "J6L", "J6R", "J7L", "J7R", "J8L", "J8R", "J9L", "J9R"};
const std::array<std::string, NUM_SERIAL_DEVICES> SERIAL_DEVICE_NAMES = {"PSU", "DMM"};

enum class SerialDevice : size_t
{
  PSU = 0u,
  DMM = 1u
};

enum class BaudRate : DWORD
{
  B4800 = CBR_4800,
  B9600 = CBR_9600,
  B19200 = CBR_19200,
  B38400 = CBR_38400,
  B57600 = CBR_57600,
  B115200 = CBR_115200
};

enum class ParityType : BYTE
{
  EVEN = EVENPARITY,
  ODD = ODDPARITY,
  NONE = NOPARITY
};

enum class ZenerSocket : size_t
{
  J5L = 0u,
  J5R = 1u,
  J6L = 2u,
  J6R = 3u,
  J7L = 4u,
  J7R = 5u,
  J8L = 6u,
  J8R = 7u,
  J9L = 8u,
  J9R = 9u
};

enum class FixtureResistor : size_t
{
  R100R = 0u,
  R1K = 1u,
  R10K = 2u,
  R100K = 3u,
  R1M = 4u
};

typedef struct
{
  uint32_t num_gpio_required;
  uint32_t diolan_serial_number;
  bool diolan_serial_present;
  std::array<std::string, NUM_SERIAL_DEVICES> com_ports;
  std::array<std::string, NUM_SERIAL_DEVICES> com_port_printables;
  std::array<BaudRate, NUM_SERIAL_DEVICES> baud_rates;
  std::array<ParityType, NUM_SERIAL_DEVICES> parities;
  std::set<ZenerSocket> zeners_installed;
  std::array<double, NUM_ZENER_SOCKETS> test_voltages;
  std::array<FixtureResistor, NUM_ZENER_SOCKETS> test_resistors;
  bool read_ok;
} test_config_t;

test_config_t parse_config_file(std::ifstream &input_stream);

#endif /* #ifndef PARSE_CONFIG_FILE_H_ */

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
