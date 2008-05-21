/* -*-c++-*-
 * arith - arithmetic instructions for Virtual M68000 Toolkit
 * Copyright (C) 1998-2008 Hypercore Software Design, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INST_ARITH_H
#define INST_ARITH_H 1

#include <vm68k/context>
#include "addressing.h"

#include <cassert>

namespace vx68k_m68k
{
  /**
   * Handles an ADDI instruction.
   */
  template<class Size, template<class Size> class D>
  struct addi_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 + v2;
      ea1.put (c, v);
      c->_status.set_cc_as_add (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };

  /**
   * Handles a CMPI instruction.
   */
  template<class Size, template<class Size> class D>
  struct cmpi_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 - v2;
      c->_status.set_cc_cmp (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };

  /**
   * Handles a SUBI instruction.
   */
  template<class Size, template<class Size> class D>
  struct subi_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 - v2;
      ea1.put (c, v);
      c->_status.set_cc_sub (v, v1, v2);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };
}

#endif
