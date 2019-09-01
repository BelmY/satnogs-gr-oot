/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2019, Libre Space Foundation <http://libre.space>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_SATNOGS_METADATA_H_
#define INCLUDE_SATNOGS_METADATA_H_

#include <string>
#include <pmt/pmt.h>
#include <json/json.h>

namespace gr {

namespace satnogs {


class metadata
{
public:
  typedef enum {
    PDU,
    CRC_VALID,
    FREQ_OFFSET,
    CORRECTED_BITS,
    TIME,
    SAMPLE_START,
    SAMPLE_CNT
  } key_t;

  static std::string
  value(const key_t& k);

  static std::string
  time_iso8601();

  static void
  add_time_iso8601(pmt::pmt_t &m);

  static void
  add_crc_valid(pmt::pmt_t &m, bool valid);

  static void
  add_sample_start(pmt::pmt_t &m, uint64_t idx);

  static void
  add_sample_cnt(pmt::pmt_t &m, uint64_t cnt);

  static Json::Value
  to_json(const pmt::pmt_t& m);
};

}  // namespace satnogs

}  // namespace gr


#endif /* INCLUDE_SATNOGS_METADATA_H_ */
