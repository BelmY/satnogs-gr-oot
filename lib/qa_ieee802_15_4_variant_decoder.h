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

#ifndef _QA_IEEE802_15_4_VARIANT_DECODER_H_
#define _QA_IEEE802_15_4_VARIANT_DECODER_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

namespace gr {
namespace satnogs {

class qa_ieee802_15_4_variant_decoder : public CppUnit::TestCase {
public:
  CPPUNIT_TEST_SUITE(qa_ieee802_15_4_variant_decoder);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST_SUITE_END();

private:
  void t1();
};

} /* namespace satnogs */
} /* namespace gr */

#endif /* _QA_IEEE802_15_4_VARIANT_DECODER_H_ */

