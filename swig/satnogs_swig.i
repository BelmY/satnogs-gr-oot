/* -*- c++ -*- */

#define SATNOGS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "satnogs_swig_doc.i"

%{
#include "satnogs/cw_matched_filter_ff.h"
#include "satnogs/morse_tree.h"
#include "satnogs/morse_decoder.h"
#include "satnogs/morse_debug_source.h"
%}

%include "satnogs/cw_matched_filter_ff.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, cw_matched_filter_ff);
%include "satnogs/morse_tree.h"
%include "satnogs/morse_decoder.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, morse_decoder);
%include "satnogs/morse_debug_source.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, morse_debug_source);
