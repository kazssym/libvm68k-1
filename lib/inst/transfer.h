/* -*-c++-*-
 * transfer - data transfer instructions for Virtual M68000 Toolkit
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

#ifndef INST_TRANSFER_H
#define INST_TRANSFER_H 1

#include <vm68k/context>
#include "addressing.h"

#include <cassert>

namespace vx68k_m68k
{
  /**
   * Handles a MOVE instruction.
   */
  template<class Size, template<class Size> class S, template<class Size> class D>
  struct move_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
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
  };

  /**
   * Handles a MOVEA instruction.  This instruction does not change CCR.
   */
  template<class Size, template<class Size> class S>
  struct movea_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::data_type data_type;
      assert (c != NULL);

      S<Size> ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      data_type v = ea1.get (c);
      c->write_reg (vm68k_data_size::LONG_WORD, vm68k_context::A0 + r2, v);

      ea1.finish (c);
      return pc + S<Size>::extension_size ();
    }
  };

  /**
   * Handles a LEA instruction.  This instruction does not change CCR.
   */
  template<template<class Size> class S>
  struct lea_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      assert (c != NULL);

      S<vm68k_word> ea1 (w & 7, pc);
      int r2 = w >> 9 & 7;

      vm68k_address_t a = ea1.address (c);
      c->write_reg (vm68k_data_size::LONG_WORD, vm68k_context::A0 + r2, a);

      return pc + S<vm68k_word>::extension_size ();
    }
  };

  /**
   * Handles a PEA instruction.  This instruction does not change CCR.
   */
  template<template<class Size> class S>
  struct pea_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      assert (c != NULL);

      S<vm68k_word> ea1 (w & 7, pc);

      vm68k_address_t a = ea1.address (c);
      c->push (vm68k_data_size::LONG_WORD, a);

      return pc + S<vm68k_word>::extension_size ();
    }
  };
}

#endif
