/**
 * SCPI_transactions.h
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

#ifndef SCPI_TRANSACTIONS_H_
#define SCPI_TRANSACTIONS_H_

#include <cinttypes>
#define NOMINMAX
#include <Windows.h>

enum class Check_ID_status : uint8_t
{
  ID_MATCH = 0u,
  ID_MISMATCH = 1u,
  ZERO_DATA = 2u,
  COM_ERROR = 3u
};

Check_ID_status check_PSU_ID(HANDLE com_port_handle, std::string &id_string);
Check_ID_status check_DMM_ID(HANDLE com_port_handle, std::string &id_string);

enum class PSC_status : uint8_t
{
  PSC_ON = 0u,
  PSC_OFF = 1u,
  ZERO_DATA = 2u,
  COM_ERROR = 3u,
  UNRECOGNIZED_RESPONSE = 4u
};

PSC_status set_PSC(HANDLE com_port_handle);

enum class OPC_status : uint8_t
{
  OPC_TRUE = 0u,
  OPC_FALSE = 1u,
  ZERO_DATA = 2u,
  COM_ERROR = 3u,
  UNRECOGNIZED_RESPONSE = 4u
};

OPC_status check_OPC(HANDLE com_port_handle);

enum class KnownStateStatus : uint8_t
{
  ALL_OK = 0u,
  ZERO_DATA = 1u,
  COM_ERROR = 2u,
  UNRECOGNIZED_RESPONSE = 3u,
  SETTING_CHECK_FAILED = 4u
};

KnownStateStatus set_known_PSU_state(HANDLE com_port_handle);
KnownStateStatus set_known_DMM_state(HANDLE com_port_handle, const bool autorange = true);

typedef struct
{
  double voltage_out_min;
  double voltage_out_max;
  double voltage_protect_min;
  double voltage_protect_max;
  bool   voltage_protect_on;
  double current_out_min;
  double current_out_max;
} PSU_limits_t;

enum class GetDataStatus : uint8_t
{
  ALL_OK = 0u,
  ZERO_DATA = 1u,
  COM_ERROR = 2u,
  UNRECOGNIZED_RESPONSE = 3u
};

GetDataStatus get_PSU_limits(HANDLE com_port_handle, PSU_limits_t &limits);
GetDataStatus meas_PSU_voltage(HANDLE com_port_handle, double &voltage);
GetDataStatus meas_PSU_DVM(HANDLE com_port_handle, double &voltage);
GetDataStatus meas_DMM_DCV(HANDLE com_port_handle, double &voltage);

enum class SetDataStatus : uint8_t
{
  ALL_OK = 0u,
  ZERO_DATA = 1u,
  COM_ERROR = 2u,
  UNRECOGNIZED_RESPONSE = 3u,
  SETTING_MISMATCH = 4u
};

SetDataStatus set_PSU_protection(HANDLE com_port_handle, double voltage_protection_level);
SetDataStatus set_PSU_current(HANDLE com_port_handle, double current_level);
SetDataStatus set_PSU_voltage(HANDLE com_port_handle, double voltage_level);
SetDataStatus set_PSU_output(HANDLE com_port_handle, bool output_on);
SetDataStatus set_PSU_sense(HANDLE com_port_handle, bool sense_remote);

#endif /* SCPI_TRANSACTIONS_H_ */

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
