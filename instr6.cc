/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2001 Hypercore Software Design, Ltd.

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

#include <vm68k/condition>
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
    using namespace condition;

    /* Handles a Bcc instruction.  */
    template <class Condition>
    uint32_type 
    _b(uint32_type pc, context &c, uint16_type w, void *)
    {
      word::svalue_type disp = w & 0xff;
      size_t extsize;
      if (disp == 0)
	{
	  disp = c.fetch_s(word(), pc + 2);
	  extsize = 2;
	}
      else
	{
	  disp = byte::svalue(disp);
	  extsize = 0;
	}
#ifdef L
      L("\tb%s %#lx\n", Condition::text(),
	long_word::uvalue(pc + 2 + disp) + 0UL);
#endif

      // This instruction does not affect the condition codes.
      Condition cond;
      if (cond(c))
	return pc + 2 + disp;

      return pc + 2 + extsize;
    }

    /* Handles a BRA instruction.  */
    uint32_type
    _bra(uint32_type pc, context &c, uint16_type w, void *)
    {
      word::svalue_type disp = w & 0xff;
      size_t len = 0;
      if (disp == 0)
	{
	  disp = c.fetch_s(word(), pc + 2);
	  len = word::aligned_value_size();
	}
      else
	disp = byte::svalue(disp);
#ifdef L
      L("\tbra %#lx\n", long_word::uvalue(pc + 2 + disp) + 0UL);
#endif

      // XXX: The condition codes are not affected.
      return pc + 2 + disp;
    }

    /* Handles a BSR instruction.  */
    uint32_type
    _bsr(uint32_type pc, context &c, uint16_type w, void *)
    {
      word::svalue_type disp = w & 0xff;
      size_t len = 0;
      if (disp == 0)
	{
	  disp = c.fetch_s(word(), pc + 2);
	  len = word::aligned_value_size();
	}
      else
	disp = byte::svalue(disp);
#ifdef L
      L("\tbsr %#lx\n", long_word::uvalue(pc + 2 + disp) + 0UL);
#endif

      // XXX: The condition codes are not affected.
      function_code fc = c.data_fc();
      long_word::put(*c.mem, fc,
			  c.regs.a[7] - long_word::aligned_value_size(),
			  pc + 2 + len);
      c.regs.a[7] -= long_word::aligned_value_size();

      return pc + 2 + disp;
    }
  }
  
  using namespace instr;

  void
  install_instructions_6(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x6000,  0xff, &_bra},
	 {0x6100,  0xff, &_bsr},
	 {0x6200,  0xff, &_b<hi>},
	 {0x6300,  0xff, &_b<ls>},
	 {0x6400,  0xff, &_b<cc>},
	 {0x6500,  0xff, &_b<cs>},
	 {0x6600,  0xff, &_b<ne>},
	 {0x6700,  0xff, &_b<eq>},
	 {0x6a00,  0xff, &_b<pl>},
	 {0x6b00,  0xff, &_b<mi>},
	 {0x6c00,  0xff, &_b<ge>},
	 {0x6d00,  0xff, &_b<lt>},
	 {0x6e00,  0xff, &_b<gt>},
	 {0x6f00,  0xff, &_b<le>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
