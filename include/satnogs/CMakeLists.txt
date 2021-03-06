# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-satnogs
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

########################################################################
# Install public header files
########################################################################
add_subdirectory(libfec)

list(APPEND DEBUG_HEADER_FILES
    cw_encoder.h
    debug_msg_source_raw.h
    debug_msg_source.h
    morse_debug_source.h
)

list(APPEND HEADER_FILES
    amsat_duv_decoder.h
    api.h
    argos_ldr_decoder.h
    ax25.h
    ax25_decoder.h
    ax25_encoder.h
    ax100_decoder.h
    base64.h
    ber_calculator.h
    coarse_doppler_correction_cc.h
    config.h
    convolutional_deinterleaver.h
    crc_async.h
    crc.h
    cw_decoder.h
    cw_to_symbol.h
    decoder.h
    doppler_correction_cc.h
    doppler_fit.h
    encoder.h
    frame_decoder.h
    frame_encoder.h
    frame_file_sink.h
    golay24.h
    ieee802_15_4_encoder.h 
    ieee802_15_4_variant_decoder.h
    iq_sink.h
    json_converter.h
    log.h
    lrpt_decoder.h
    lrpt_sync.h
    metadata.h
    morse.h
    moving_sum.h
    morse_decoder.h
    morse_tree.h
    multi_format_msg_sink.h
    noaa_apt_sink.h
    ogg_encoder.h
    ogg_source.h
    rs_encoder.h
    shift_reg.h
    sstv_pd120_sink.h
    tcp_rigctl_msg_source.h
    udp_msg_sink.h
    udp_msg_source.h
    utils.h
    waterfall_sink.h
    whitening.h
)

if(${INCLUDE_DEBUG_BLOCKS})
    list(APPEND HEADER_FILES ${DEBUG_HEADER_FILES})
endif()

install(FILES
    ${HEADER_FILES}
    DESTINATION include/satnogs
)
