# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

add_subdirectory(satellites)

set(flowgraphs
    afsk1200_ax25.grc
    bpsk_ax25.grc
    cw_decoder.grc
    example_flowgraph.grc
    fsk_ax25.grc
    iq_receiver.grc
    sstv_pd120_demod.grc
)
message(“Compiling GRC SatNOGS flowgraphs…”)
foreach(grc_file ${flowgraphs})

    message("Compiling " ${grc_file})
    execute_process(COMMAND grcc ${grc_file}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

GR_PYTHON_INSTALL(
    PROGRAMS
    satnogs_afsk1200_ax25.py
    satnogs_bpsk_ax25.py
    satnogs_cw_decoder.py
    satnogs_example_flowgraph.py
    satnogs_fsk_ax25.py
    satnogs_iq_receiver.py
    satnogs_sstv_pd120_demod.py
    DESTINATION bin
)
