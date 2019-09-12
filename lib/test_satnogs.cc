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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestResult.h>

#include <gnuradio/unittests.h>
#include "qa_satnogs.h"
#include <iostream>
#include <fstream>

int
main(int argc, char **argv)
{
  CppUnit::TestResultCollector result;
  CppUnit::BriefTestProgressListener progressListener;
  CppUnit::TextTestRunner runner;

  runner.addTest(qa_satnogs::suite());
  runner.eventManager().addListener(&result);
  runner.eventManager().addListener(&progressListener);

  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
}
