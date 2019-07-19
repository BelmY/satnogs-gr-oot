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

#ifndef LIB_QA_GOLAY24_H_
#define LIB_QA_GOLAY24_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <satnogs/golay24.h>

namespace gr {

namespace satnogs {

class qa_golay24 : public CppUnit::TestCase
{
public:
  CPPUNIT_TEST_SUITE(qa_golay24);
  CPPUNIT_TEST(example_47);
  CPPUNIT_TEST(errors_0);
  CPPUNIT_TEST(errors_1);
  CPPUNIT_TEST(errors_3);
  CPPUNIT_TEST(errors_4);
  CPPUNIT_TEST_SUITE_END();

private:
  void
  example_47();

  void
  errors_0 ();

  void
  errors_1 ();

  void
  errors_3 ();

  void
  errors_4 ();
};

}  // namespace satnogs

}  // namespace gr



#endif /* LIB_QA_GOLAY24_H_ */
