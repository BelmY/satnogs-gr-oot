/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, 2019, Libre Space Foundation <http://libre.space>
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

#ifndef INCLUDED_SATNOGS_MORSE_TREE_H
#define INCLUDED_SATNOGS_MORSE_TREE_H

#include <satnogs/api.h>
#include <string>
#include <satnogs/morse.h>

namespace gr {
namespace satnogs {

/*!
 * \brief Binary tree node containing the corresponding character
 */
class SATNOGS_API tree_node {
private:
  const char d_char;
  tree_node *d_left;
  tree_node *d_right;

public:
  tree_node(char c);

  void
  set_left_child(tree_node *child);

  void
  set_right_child(tree_node *child);

  tree_node *
  get_left_child();

  tree_node *
  get_right_child();

  char
  get_char();
};

/*!
 * \brief A Binary tree representation of the Morse coding scheme.
 * Left transitions occur when a dot is received, whereas right transitions
 * are performed during the reception of a dash.
 *
 * The tree follows the ITU International Morse code representation
 * ITU-R M.1677-1
 */
class SATNOGS_API morse_tree {
public:
  morse_tree(size_t max_len = 128);
  morse_tree(char unrecognized, size_t max_len = 128);
  ~morse_tree();
  void
  reset();
  bool
  received_symbol(morse_symbol_t s);
  std::string
  get_word();
  size_t
  get_max_word_len() const;
  size_t
  get_word_len();

private:
  const char d_unrecognized_symbol;
  tree_node *d_root;
  tree_node *d_current;
  const size_t d_buff_len;
  size_t d_word_len;
  char *d_word_buffer;

  void
  construct_tree();
  void
  delete_tree(tree_node *node);
};

} // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_TREE_H */

