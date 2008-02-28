/* -*- C++ -*- */
/* Monadic instructions for Virtual M68000 Toolkit
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

#ifndef INST_MONADIC_H
#define INST_MONADIC_H 1

#include <cassert>

namespace vx68k_m68k
{
  namespace inst
  {
namespace vx68k_m68k
{
  namespace inst
  {
    /* Handles a CLR instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_CLR (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      D<Size> ea1 (w & 7, pc);

      data_type v = 0;
      ea1.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      return pc + D<Size>::extension_size ();
    }

    /* Handles a NEG instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_NEG (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      D<Size> ea1 (w & 7, pc);

      data_type v1 = ea1.get (c);
      data_type v = -v1;
      ea1.put (c, v);
      c->_status.set_cc_sub (v, 0, v1);

      ea1.finish (c);
      return pc + D<Size>::extension_size ();
    }

    /* Handles a NOT instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_NOT (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      D<Size> ea1 (w & 7, pc);

      data_type v1 = ea1.get (c);
      data_type v = ~v1;
      ea1.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      return pc + D<Size>::extension_size ();
    }

    /* Handles a TST instruction.  */
    template<class Size, template<class Size> class D>
    address_t do_TST (address_t pc, udata_fast16_t w, execution_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      D<Size> ea1 (w & 7, pc);

      data_type v1 = ea1.get(c);
      c->_status.set_cc (v1);

      ea1.finish(c);
      return pc + D<Size>::extension_size();
    }
  }
}

#endif
