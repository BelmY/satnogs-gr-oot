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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "json_converter_impl.h"
#include <satnogs/metadata.h>
#include <json/json.h>


namespace gr {
namespace satnogs {

json_converter::sptr
json_converter::make(const std::string &extra)
{
  return gnuradio::get_initial_sptr(new json_converter_impl(extra));
}

/*
 * The private constructor
 */
json_converter_impl::json_converter_impl(const std::string &extra) :
  gr::block("json_converter", gr::io_signature::make(0, 0, 0),
            gr::io_signature::make(0, 0, 0)),
  d_extra(extra)
{
  message_port_register_in(pmt::mp("in"));
  message_port_register_out(pmt::mp("out"));

  set_msg_handler(pmt::mp("in"),
                  boost::bind(&json_converter_impl::convert, this, _1));
}

/*
 * Our virtual destructor.
 */
json_converter_impl::~json_converter_impl()
{
}

void
json_converter_impl::convert(pmt::pmt_t m)
{
  Json::CharReaderBuilder crb;
  Json::CharReader *cr = crb.newCharReader();
  Json::Value extra;
  std::string err;


  Json::Value root = metadata::to_json(m);
  if (cr->parse(d_extra.c_str(), d_extra.c_str() + d_extra.size(), &extra,
                &err)) {
    root["extra"] = extra;
  }
  const std::string &s = root.toStyledString();
  const char *c = s.c_str();
  message_port_pub(pmt::mp("out"), pmt::make_blob(c, s.length()));
}

} /* namespace satnogs */
} /* namespace gr */

