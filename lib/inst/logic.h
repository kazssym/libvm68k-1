/* -*- C++ -*- */
/* Logical instructions for Virtual M68000 Toolkit
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

#ifndef INST_LOGIC_H
#define INST_LOGIC_H 1

#include <vm68k/context>
#include "addressing.h"

#include <cassert>

namespace vx68k_m68k
{
  /**
   * Handles an ANDI instruction.
   */
  template<class Size, template<class Size> class D>
  struct andi_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 & v2;
      ea1.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };

  /**
   * Handles an ANDI-to-CCR instruction.
   */
  struct andi_to_ccr_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef vm68k_byte::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (vm68k_data_size::BYTE, pc);

      udata_type v1 = c->_status & 0xff;
      udata_type v = v1 & v2;
      c->_status = c->_status & ~0xff | v & 0xff; // FIXME

      return pc + vm68k_byte::aligned_data_size ();
    }
  };

  /**
   * Handles an ANDI-to-SR instruction.
   */
  struct andi_to_sr_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t,
                                    vm68k_context *c)
    {
      typedef vm68k_word::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (vm68k_data_size::WORD, pc);

      // This instruction is privileged.
      if (!c->super ())
        {
          throw privilege_violation_exception (pc);
        }

      udata_type v1 = c->status ();
      udata_type v = v1 & v2;
      c->set_status (v);

      return pc + vm68k_word::aligned_data_size ();
    }
  };

  /**
   * Handles an EORI instruction.
   */
  template<class Size, template<class Size> class D>
  struct eori_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 ^ v2;
      ea1.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };

  /**
   * Handles an ORI instruction.
   */
  template<class Size, template<class Size> class D>
  struct ori_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t w,
                                    vm68k_context *c)
    {
      typedef typename Size::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (Size (), pc);
      D<Size> ea1 (w & 7, pc + Size::aligned_data_size ());

      udata_type v1 = ea1.get (c);
      udata_type v = v1 | v2;
      ea1.put (c, v);
      c->_status.set_cc (v);

      ea1.finish (c);
      return pc + Size::aligned_data_size () + D<Size>::extension_size ();
    }
  };

  /**
   * Handles an ORI-to-CCR instruction.
   */
  struct ori_to_ccr_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t,
                                    vm68k_context *c)
    {
      typedef vm68k_byte::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (vm68k_data_size::BYTE, pc);

      udata_type v1 = c->_status & 0xff;
      udata_type v = v1 | v2;
      c->_status = c->_status & ~0xff | v & 0xff; // FIXME

      return pc + vm68k_byte::aligned_data_size ();
    }
  };

  /**
   * Handles an ORI-to-SR instruction.
   */
  struct ori_to_sr_instruction
  {
    static vm68k_address_t execute (vm68k_address_t pc, uint_fast16_t,
                                    vm68k_context *c)
    {
      typedef vm68k_word::udata_type udata_type;
      assert (c != NULL);

      udata_type v2 = c->fetch_unsigned (vm68k_data_size::WORD, pc);

      // This instruction is privileged.
      if (!c->super ())
        {
          throw privilege_violation_exception (pc);
        }

      udata_type v1 = c->status ();
      udata_type v = v1 | v2;
      c->set_status (v);

      return pc + vm68k_word::aligned_data_size ();
    }
  };
}

#endif
