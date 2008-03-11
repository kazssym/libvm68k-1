/* -*- C++ -*- */
/* Control instructions for Virtual M68000 Toolkit
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

#ifndef INST_CONTROL_H
#define INST_CONTROL_H 1

#include <cassert>

namespace vx68k_m68k
{
  namespace inst
  {
    /**
     * Handles a JMP instruction.  This instruction does not change CCR.
     */
    template<template<class Size> class S>
    address_t do_JMP (address_t pc, udata_fast16_t w, execution_context *c)
    {
      assert (c != NULL);

      S<word_size> ea1 (w & 7, pc);

      address_t a = ea1.address (c);

      return a;
    }

    /**
     * Handles a JSR instruction.  This instruction does not change CCR.
     */
    template<template<class Size> class S>
    address_t do_JSR (address_t pc, udata_fast16_t w, execution_context *c)
    {
      assert (c != NULL);

      S<word_size> ea1 (w & 7, pc);

      address_t a = ea1.address (c);
      c->push (long_word, pc + S<word_size>::extension_size ());

      return a;
    }
  }
}

#endif
