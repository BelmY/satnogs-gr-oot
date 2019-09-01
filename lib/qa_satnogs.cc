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

#include "qa_satnogs.h"
#include "qa_golay24.h"
#include "qa_ax25_decoder.h"
#include "qa_json_converter.h"

CppUnit::TestSuite *
qa_satnogs::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("satnogs");
  s->addTest(gr::satnogs::qa_golay24::suite());
  s->addTest(gr::satnogs::qa_ax25_decoder::suite());
  s->addTest(gr::satnogs::qa_json_converter::suite());
  return s;
}
