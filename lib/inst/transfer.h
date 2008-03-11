/* -*- C++ -*- */
/* Data transfer instructions for Virtual M68000 Toolkit
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

#ifndef INST_XFER_H
#define INST_XFER_H 1

#include <cassert>

namespace vx68k_m68k
{
  namespace inst
  {
    /* Handles a MOVE instruction.  */
    template <class Size, template<class Size> class S,
              template<class Size> class D>
    address_t do_MOVE (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      S<Size> ea1 (w & 7, pc);
      D<Size> ea2 (w >> 9 & 7, pc + S<Size>::extension_size ());

      data_type v = ea1.get (c);
      ea2.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      ea2.finish (c);
      return pc + S<Size>::extension_size () + D<Size>::extension_size ();
    }

    /* Handles a MOVEA instruction.  */
    template <class Size, template<class Size> class S>
    address_t do_MOVEA (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      S<Size> ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      data_type v = ea1.get (c);
      c->write_reg (long_word, A0 + r2, v);
      // XXX: This instruction does not affect CCR.

      ea1.finish (c);
      return pc + S<Size>::extension_size ();
    }
  }
}

#endif
