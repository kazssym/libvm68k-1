/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2000 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#undef const
#undef inline

#include "instr.h"

#include <vm68k/size>
#include <cstdio>

#ifdef HAVE_NANA_H
# include <nana.h>
#else
# include <cassert>
# define I assert
#endif

using namespace std;

#ifdef HAVE_NANA_H
extern bool nana_instruction_trace;
# undef L_DEFAULT_GUARD
# define L_DEFAULT_GUARD nana_instruction_trace
#endif

namespace vm68k
{
  namespace instr
  {
    /* Handles a MOVEQ instruction.  */
    uint32_type
    _moveq(uint32_type pc, context &c, uint16_type w, void *)
    {
      long_word::sint_type value = normal_s(byte(), w);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tmoveq%s #%#x,%%d%u\n", long_word::suffix(),
	byte::normal_u(value), reg2);
#endif

      long_word::put((&c.d0)[reg2], value);
      c.ccr.set_cc(value);

      return pc + 2;
    }
  }
  
  using namespace instr;

  void
  install_instructions_7(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x7000, 0xeff, &_moveq}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
