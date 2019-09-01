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

#include <satnogs/metadata.h>
#include <satnogs/base64.h>
#include <satnogs/date.h>
#include <stdexcept>
#include <chrono>

namespace gr {

namespace satnogs {

std::string
metadata::value(const key_t& k)
{
  switch(k) {
    case PDU:
      return "pdu";
    case CRC_VALID:
      return "crc_valid";
    case FREQ_OFFSET:
      return "freq_offset";
    case CORRECTED_BITS:
      return "corrected_bits";
    case TIME:
      return "time";
    case SAMPLE_START:
      return "sample_start";
    case SAMPLE_CNT:
      return "sample_cnt";
    default:
      throw std::invalid_argument("metadata: invalid key");
  }
}

/**
 *
 * @return string with the date in ISO 8601 format
 */
std::string
metadata::time_iso8601()
{
  /* check for the current UTC time */
  std::chrono::system_clock::time_point tp =
      std::chrono::system_clock::now ();

  return date::format("%FT%TZ", date::floor<std::chrono::microseconds>(tp));
}

/**
 * Adds the system current time in ISO 8601  sting format
 * @param m reference to a PMT dictionary
 */
void
metadata::add_time_iso8601(pmt::pmt_t &m)
{
  m = pmt::dict_add(m, pmt::mp(value(TIME)), pmt::mp(time_iso8601()));
}

/**
 * Adds the CRC validity value
 * @param m reference to a PMT dictionary
 */
void
metadata::add_crc_valid(pmt::pmt_t &m, bool valid)
{
  m = pmt::dict_add(m, pmt::mp(value(CRC_VALID)), pmt::from_bool(valid));
}

void
metadata::add_sample_start(pmt::pmt_t &m, uint64_t idx)
{
  m = pmt::dict_add(m, pmt::mp(value(SAMPLE_START)), pmt::from_uint64(idx));
}

void
metadata::add_sample_cnt(pmt::pmt_t &m, uint64_t cnt)
{
  m = pmt::dict_add(m, pmt::mp(value(SAMPLE_CNT)), pmt::from_uint64(cnt));
}

Json::Value
metadata::to_json(const pmt::pmt_t& m)
{
  Json::Value root;
  pmt::pmt_t v = pmt::dict_ref(m, pmt::mp(value(PDU)), pmt::PMT_NIL);
  if(!pmt::equal(v, pmt::PMT_NIL)) {
    uint8_t *b = (uint8_t *) pmt::blob_data(v);
    size_t len = pmt::blob_length(v);
    root[value(PDU)] = base64_encode(b, len);
  }

  v = pmt::dict_ref(m, pmt::mp(value(TIME)), pmt::PMT_NIL);
  if(!pmt::equal(v, pmt::PMT_NIL)) {
    root[value(TIME)] = pmt::symbol_to_string(v);
  }

  v = pmt::dict_ref (m, pmt::mp (value (CRC_VALID)), pmt::PMT_NIL);
  if (!pmt::equal (v, pmt::PMT_NIL)) {
    root[value (CRC_VALID)] = pmt::to_bool (v);
  }

  v = pmt::dict_ref (m, pmt::mp (value (SAMPLE_START)), pmt::PMT_NIL);
  if (!pmt::equal (v, pmt::PMT_NIL)) {
    root[value (SAMPLE_START)] = Json::Value::UInt64(pmt::to_uint64 (v));
  }
  return root;
}

}  // namespace satnogs

}  // namespace gr
