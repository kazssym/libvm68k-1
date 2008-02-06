/* -*- C++ -*- */
/* Bit instructions for Virtual M68000 Toolkit
   Copyright (C) 1998-2008 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.  */

#ifndef INST_BIT_H
#define INST_BIT_H 1

#include <vm68k/size>

#include <cassert>

namespace vx68k_m68k
{
  namespace inst
  {
    /* Handles a BCLR instruction (data register).  */
    template<class D>
    address_t inst_bclr_d (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      D ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      int v2 =
        c->read_reg_unsigned (word, D0 + r2) & size_type::data_size () * 8 - 1;
      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      ea1.put (c, v1 & ~mask);
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);  // FIXME.

      ea1.finish (c);
      return pc + D::extension_size ();
    }

    /* Handles a BCLR instruction (immediate).  */
    template<class D>
    address_t inst_bclr_i (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      int v2 = c->fetch_unsigned (word, pc) & size_type::data_size () * 8 - 1;
      D ea1 (w & 7, pc + word_size::aligned_data_size ());

      // This instruction affects only the Z bit of the condition codes.
      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      ea1.put (c, v1 & ~mask);
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);    // FIXME.

      ea1.finish (c);
      return pc + word_size::aligned_data_size () + D::extension_size();
    }

    /* Handles a BSET instruction (data register).  */
    template<class D>
    address_t inst_bset_d (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      D ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      int v2 =
        c->read_reg_unsigned (word, D0 + r2) & size_type::data_size () * 8 - 1;
      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      ea1.put (c, v1 | mask);
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);  // FIXME.

      ea1.finish (c);
      return pc + D::extension_size ();
    }

    /* Handles a BSET instruction (immediate).  */
    template<class D>
    address_t inst_bset_i (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      int v2 = c->fetch_unsigned (word, pc) & size_type::data_size () * 8 - 1;
      D ea1 (w & 7, pc + word_size::aligned_data_size ());

      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      ea1.put (c, v1 | mask);
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);    // FIXME.

      ea1.finish (c);
      return pc + word_size::aligned_data_size () + D::extension_size ();
    }

    /* Handles a BTST instruction (data register).  */
    template<class D>
    address_t inst_btst_d (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      D ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      int v2 =
        c->read_reg_unsigned (word, D0 + r2) & size_type::data_size () * 8 - 1;
      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);    // FIXME.

      ea1.finish (c);
      return pc + D::extension_size ();
    }

    /* Handles a BTST instruction (immediate).  */
    template<class D>
    address_t inst_btst_i (address_t pc, udata_fast16_t w,
                           execution_context *c)
    {
      typedef typename D::size_type size_type;
      typedef typename D::data_type data_type;
      assert (c != NULL);

      int v2 = c->fetch_unsigned (word, pc) & size_type::data_size() * 8 - 1;
      D ea1 (w & 7, pc + word_size::aligned_data_size ());

      data_type mask = (data_type) 1 << v2;
      data_type v1 = ea1.get (c);
      bool test = (v1 & mask) != 0;
      // XXX: This instruction affects only the Z bit of CCR.
      c->_status.set_cc (test);   // FIXME.

      ea1.finish (c);
      return pc + word_size::aligned_data_size () + D::extension_size ();
    }
  }
}

#endif
