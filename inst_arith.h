/* -*- C++ -*- */
/* Arithmetic instructions for Virtual M68000 Toolkit
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

#ifndef INST_ARITH_H
#define INST_ARITH_H 1

#include <cassert>

namespace vx68k_m68k
{
  namespace inst
  {
    /* Handles an ADDI instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_ADDI (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename D<Size>::data_type data_type;
      assert (c != NULL);

      data_type v2 = c->fetch (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      data_type v1 = ea1.get (c);
      data_type v = v1 + v2;
      ea1.put (c, v);
      c->_status.set_cc_as_add (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }

    /* Handles a CMPI instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_CMPI (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename D<Size>::data_type data_type;
      assert (c != NULL);

      data_type v2 = c->fetch (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      data_type v1 = ea1.get (c);
      data_type v = v1 - v2;
      c->_status.set_cc_cmp (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }

    /* Handles a SUBI instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_SUBI (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename D<Size>::data_type data_type;
      assert (c != NULL);

      data_type v2 = c->fetch (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      data_type v1 = ea1.get (c);
      data_type v = v1 - v2;
      ea1.put (c, v);
      c->_status.set_cc_sub (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  }
}

#endif
