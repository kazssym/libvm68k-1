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
# include "config.h"
#endif
#undef const
#undef inline

#include "instr.h"

#include <vm68k/add.h>
#include <vm68k/conditional.h>
#include <vm68k/processor.h>

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
    using namespace conditional;
    using namespace add;

    /* Handles an ADDQ instruction.  */
    template <class Size, class Destination>
    uint32_type
    _addq(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Destination ea1(w & 7, pc + 2);
      int value2 = w >> 9 & 7;
      if (value2 == 0)
	value2 = 8;
#ifdef L
      L("\taddq%s #%d,%s\n", Size::suffix(), value2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::svalue(value1 + value2);
      ea1.put(c, value);
      c.regs.ccr.set_cc_as_add(value, value1, value2);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles an ADDQ instruction (address register).  */
    template <class Size>
    uint32_type
    _addq_a(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      int reg1 = w & 7;
      int value2 = w >> 9 & 7;
      if (value2 == 0)
	value2 = 8;
#ifdef L
      L("\taddq%s #%d,%%a%u\n", Size::suffix(), value2, reg1);
#endif

      // The condition codes are not affected by this instruction.
      long_word_size::svalue_type value1 = long_word_size::get(c.regs.a[reg1]);
      long_word_size::svalue_type value
	= long_word_size::svalue(value1 + value2);
      long_word_size::put(c.regs.a[reg1], value);

      return pc + 2;
    }

    /* Handles a DBcc instruction.  */
    template <class Condition>
    uint32_type
    _db(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Condition cond;
      int reg1 = w & 7;
      word_size::svalue_type disp = c.fetch_s(word_size(), pc + 2);
#ifdef L
      L("\tdb%s %%d%u,%#lx\n", Condition::text(), reg1,
	long_word_size::uvalue(pc + 2 + disp) + 0UL);
#endif

      // This instruction does not affect the condition codes.
      if (!cond(c))
	{
	  sint16_type value = word_size::get(c.regs.d[reg1]);
	  value = word_size::svalue(value - 1);
	  word_size::put(c.regs.d[reg1], value);
	  if (value != -1)
	    return pc + 2 + disp;
	}

      return pc + 2 + 2;
    }

    /* Handles a Scc instruction.  */
    template <class Condition, class Destination>
    uint32_type
    _s(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\ts%s%s %s\n", Condition::text(), byte_size::suffix(),
	ea1.text(c).c_str());
#endif

      // The condition codes are not affected by this instruction.
      Condition cond;
      ea1.put(c, cond(c) ? ~0 : 0);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }

    /* Handles a SUBQ instruction.  */
    template <class Size, class Destination>
    uint32_type
    _subq(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Destination ea1(w & 7, pc + 2);
      int value2 = w >> 9 & 7;
      if (value2 == 0)
	value2 = 8;
#ifdef L
      L("\tsubq%s #%d,%s\n", Size::suffix(), value2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::svalue(value1 - value2);
      ea1.put(c, value);
      c.regs.ccr.set_cc_sub(value, value1, value2);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a SUBQ instruction (address register).  */
    template <class Size>
    uint32_type
    _subq_a(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      int reg1 = w & 7;
      int value2 = w >> 9 & 7;
      if (value2 == 0)
	value2 = 8;
#ifdef L
      L("\tsubq%s #%d,%%a%u\n", Size::suffix(), value2, reg1);
#endif

      // This instruction does not affect the condition codes.
      long_word_size::svalue_type value1 = long_word_size::get(c.regs.a[reg1]);
      long_word_size::svalue_type value
	= long_word_size::svalue(value1 - value2);
      long_word_size::put(c.regs.a[reg1], value);

      return pc + 2;
    }
  }
  
  using namespace instr;

  void
  install_instructions_5(processor &p, unsigned long d)
  {
    static const instruction_map inst[]
      = {{0x5000, 0xe07, &_addq<byte_size, byte_d_register>},
	 {0x5010, 0xe07, &_addq<byte_size, byte_indirect>},
	 {0x5018, 0xe07, &_addq<byte_size, byte_postinc_indirect>},
	 {0x5020, 0xe07, &_addq<byte_size, byte_predec_indirect>},
	 {0x5028, 0xe07, &_addq<byte_size, byte_disp_indirect>},
	 {0x5030, 0xe07, &_addq<byte_size, byte_index_indirect>},
	 {0x5038, 0xe00, &_addq<byte_size, byte_abs_short>},
	 {0x5039, 0xe00, &_addq<byte_size, byte_abs_long>},
	 {0x5040, 0xe07, &_addq<word_size, word_d_register>},
	 {0x5048, 0xe07, &_addq_a<word_size>},
	 {0x5050, 0xe07, &_addq<word_size, word_indirect>},
	 {0x5058, 0xe07, &_addq<word_size, word_postinc_indirect>},
	 {0x5060, 0xe07, &_addq<word_size, word_predec_indirect>},
	 {0x5068, 0xe07, &_addq<word_size, word_disp_indirect>},
	 {0x5070, 0xe07, &_addq<word_size, word_index_indirect>},
	 {0x5078, 0xe00, &_addq<word_size, word_abs_short>},
	 {0x5079, 0xe00, &_addq<word_size, word_abs_long>},
	 {0x5080, 0xe07, &_addq<long_word_size, long_word_d_register>},
	 {0x5088, 0xe07, &_addq_a<long_word_size>},
	 {0x5090, 0xe07, &_addq<long_word_size, long_word_indirect>},
	 {0x5098, 0xe07, &_addq<long_word_size, long_word_postinc_indirect>},
	 {0x50a0, 0xe07, &_addq<long_word_size, long_word_predec_indirect>},
	 {0x50a8, 0xe07, &_addq<long_word_size, long_word_disp_indirect>},
	 {0x50b0, 0xe07, &_addq<long_word_size, long_word_index_indirect>},
	 {0x50b8, 0xe00, &_addq<long_word_size, long_word_abs_short>},
	 {0x50b9, 0xe00, &_addq<long_word_size, long_word_abs_long>},
	 {0x50c0,     7, &_s<t, byte_d_register>},
	 {0x50c8,     7, &_db<t>},
	 {0x50d0,     7, &_s<t, byte_indirect>},
	 {0x50d8,     7, &_s<t, byte_postinc_indirect>},
	 {0x50e0,     7, &_s<t, byte_predec_indirect>},
	 {0x50e8,     7, &_s<t, byte_disp_indirect>},
	 {0x50f0,     7, &_s<t, byte_index_indirect>},
	 {0x50f8,     0, &_s<t, byte_abs_short>},
	 {0x50f9,     0, &_s<t, byte_abs_long>},
	 {0x5100, 0xe07, &_subq<byte_size, byte_d_register>},
	 {0x5110, 0xe07, &_subq<byte_size, byte_indirect>},
	 {0x5118, 0xe07, &_subq<byte_size, byte_postinc_indirect>},
	 {0x5120, 0xe07, &_subq<byte_size, byte_predec_indirect>},
	 {0x5128, 0xe07, &_subq<byte_size, byte_disp_indirect>},
	 {0x5130, 0xe07, &_subq<byte_size, byte_index_indirect>},
	 {0x5138, 0xe00, &_subq<byte_size, byte_abs_short>},
	 {0x5139, 0xe00, &_subq<byte_size, byte_abs_long>},
	 {0x5140, 0xe07, &_subq<word_size, word_d_register>},
	 {0x5148, 0xe07, &_subq_a<word_size>},
	 {0x5150, 0xe07, &_subq<word_size, word_indirect>},
	 {0x5158, 0xe07, &_subq<word_size, word_postinc_indirect>},
	 {0x5160, 0xe07, &_subq<word_size, word_predec_indirect>},
	 {0x5168, 0xe07, &_subq<word_size, word_disp_indirect>},
	 {0x5170, 0xe07, &_subq<word_size, word_index_indirect>},
	 {0x5178, 0xe00, &_subq<word_size, word_abs_short>},
	 {0x5179, 0xe00, &_subq<word_size, word_abs_long>},
	 {0x5180, 0xe07, &_subq<long_word_size, long_word_d_register>},
	 {0x5188, 0xe07, &_subq_a<long_word_size>},
	 {0x5190, 0xe07, &_subq<long_word_size, long_word_indirect>},
	 {0x5198, 0xe07, &_subq<long_word_size, long_word_postinc_indirect>},
	 {0x51a0, 0xe07, &_subq<long_word_size, long_word_predec_indirect>},
	 {0x51a8, 0xe07, &_subq<long_word_size, long_word_disp_indirect>},
	 {0x51b0, 0xe07, &_subq<long_word_size, long_word_index_indirect>},
	 {0x51b8, 0xe00, &_subq<long_word_size, long_word_abs_short>},
	 {0x51b9, 0xe00, &_subq<long_word_size, long_word_abs_long>},
	 {0x51c0,     7, &_s<f, byte_d_register>},
	 {0x51c8,     7, &_db<f>},
	 {0x51d0,     7, &_s<f, byte_indirect>},
	 {0x51d8,     7, &_s<f, byte_postinc_indirect>},
	 {0x51e0,     7, &_s<f, byte_predec_indirect>},
	 {0x51e8,     7, &_s<f, byte_disp_indirect>},
	 {0x51f0,     7, &_s<f, byte_index_indirect>},
	 {0x51f8,     0, &_s<f, byte_abs_short>},
	 {0x51f9,     0, &_s<f, byte_abs_long>},
	 {0x52c0,     7, &_s<hi, byte_d_register>},
	 {0x52c8,     7, &_db<hi>},
	 {0x52d0,     7, &_s<hi, byte_indirect>},
	 {0x52d8,     7, &_s<hi, byte_postinc_indirect>},
	 {0x52e0,     7, &_s<hi, byte_predec_indirect>},
	 {0x52e8,     7, &_s<hi, byte_disp_indirect>},
	 {0x52f0,     7, &_s<hi, byte_index_indirect>},
	 {0x52f8,     0, &_s<hi, byte_abs_short>},
	 {0x52f9,     0, &_s<hi, byte_abs_long>},
	 {0x53c0,     7, &_s<ls, byte_d_register>},
	 {0x53c8,     7, &_db<ls>},
	 {0x53d0,     7, &_s<ls, byte_indirect>},
	 {0x53d8,     7, &_s<ls, byte_postinc_indirect>},
	 {0x53e0,     7, &_s<ls, byte_predec_indirect>},
	 {0x53e8,     7, &_s<ls, byte_disp_indirect>},
	 {0x53f0,     7, &_s<ls, byte_index_indirect>},
	 {0x53f8,     0, &_s<ls, byte_abs_short>},
	 {0x53f9,     0, &_s<ls, byte_abs_long>},
	 {0x54c0,     7, &_s<cc, byte_d_register>},
	 {0x54c8,     7, &_db<cc>},
	 {0x54d0,     7, &_s<cc, byte_indirect>},
	 {0x54d8,     7, &_s<cc, byte_postinc_indirect>},
	 {0x54e0,     7, &_s<cc, byte_predec_indirect>},
	 {0x54e8,     7, &_s<cc, byte_disp_indirect>},
	 {0x54f0,     7, &_s<cc, byte_index_indirect>},
	 {0x54f8,     0, &_s<cc, byte_abs_short>},
	 {0x54f9,     0, &_s<cc, byte_abs_long>},
	 {0x55c0,     7, &_s<cs, byte_d_register>},
	 {0x55c8,     7, &_db<cs>},
	 {0x55d0,     7, &_s<cs, byte_indirect>},
	 {0x55d8,     7, &_s<cs, byte_postinc_indirect>},
	 {0x55e0,     7, &_s<cs, byte_predec_indirect>},
	 {0x55e8,     7, &_s<cs, byte_disp_indirect>},
	 {0x55f0,     7, &_s<cs, byte_index_indirect>},
	 {0x55f8,     0, &_s<cs, byte_abs_short>},
	 {0x55f9,     0, &_s<cs, byte_abs_long>},
	 {0x56c0,     7, &_s<ne, byte_d_register>},
	 {0x56c8,     7, &_db<ne>},
	 {0x56d0,     7, &_s<ne, byte_indirect>},
	 {0x56d8,     7, &_s<ne, byte_postinc_indirect>},
	 {0x56e0,     7, &_s<ne, byte_predec_indirect>},
	 {0x56e8,     7, &_s<ne, byte_disp_indirect>},
	 {0x56f0,     7, &_s<ne, byte_index_indirect>},
	 {0x56f8,     0, &_s<ne, byte_abs_short>},
	 {0x56f9,     0, &_s<ne, byte_abs_long>},
	 {0x57c0,     7, &_s<eq, byte_d_register>},
	 {0x57c8,     7, &_db<eq>},
	 {0x57d0,     7, &_s<eq, byte_indirect>},
	 {0x57d8,     7, &_s<eq, byte_postinc_indirect>},
	 {0x57e0,     7, &_s<eq, byte_predec_indirect>},
	 {0x57e8,     7, &_s<eq, byte_disp_indirect>},
	 {0x57f0,     7, &_s<eq, byte_index_indirect>},
	 {0x57f8,     0, &_s<eq, byte_abs_short>},
	 {0x57f9,     0, &_s<eq, byte_abs_long>},
	 {0x5ac0,     7, &_s<pl, byte_d_register>},
	 {0x5ac8,     7, &_db<pl>},
	 {0x5ad0,     7, &_s<pl, byte_indirect>},
	 {0x5ad8,     7, &_s<pl, byte_postinc_indirect>},
	 {0x5ae0,     7, &_s<pl, byte_predec_indirect>},
	 {0x5ae8,     7, &_s<pl, byte_disp_indirect>},
	 {0x5af0,     7, &_s<pl, byte_index_indirect>},
	 {0x5af8,     0, &_s<pl, byte_abs_short>},
	 {0x5af9,     0, &_s<pl, byte_abs_long>},
	 {0x5bc0,     7, &_s<mi, byte_d_register>},
	 {0x5bc8,     7, &_db<mi>},
	 {0x5bd0,     7, &_s<mi, byte_indirect>},
	 {0x5bd8,     7, &_s<mi, byte_postinc_indirect>},
	 {0x5be0,     7, &_s<mi, byte_predec_indirect>},
	 {0x5be8,     7, &_s<mi, byte_disp_indirect>},
	 {0x5bf0,     7, &_s<mi, byte_index_indirect>},
	 {0x5bf8,     0, &_s<mi, byte_abs_short>},
	 {0x5bf9,     0, &_s<mi, byte_abs_long>},
	 {0x5cc0,     7, &_s<ge, byte_d_register>},
	 {0x5cc8,     7, &_db<ge>},
	 {0x5cd0,     7, &_s<ge, byte_indirect>},
	 {0x5cd8,     7, &_s<ge, byte_postinc_indirect>},
	 {0x5ce0,     7, &_s<ge, byte_predec_indirect>},
	 {0x5ce8,     7, &_s<ge, byte_disp_indirect>},
	 {0x5cf0,     7, &_s<ge, byte_index_indirect>},
	 {0x5cf8,     0, &_s<ge, byte_abs_short>},
	 {0x5cf9,     0, &_s<ge, byte_abs_long>},
	 {0x5dc0,     7, &_s<lt, byte_d_register>},
	 {0x5dc8,     7, &_db<lt>},
	 {0x5dd0,     7, &_s<lt, byte_indirect>},
	 {0x5dd8,     7, &_s<lt, byte_postinc_indirect>},
	 {0x5de0,     7, &_s<lt, byte_predec_indirect>},
	 {0x5de8,     7, &_s<lt, byte_disp_indirect>},
	 {0x5df0,     7, &_s<lt, byte_index_indirect>},
	 {0x5df8,     0, &_s<lt, byte_abs_short>},
	 {0x5df9,     0, &_s<lt, byte_abs_long>},
	 {0x5ec0,     7, &_s<gt, byte_d_register>},
	 {0x5ec8,     7, &_db<gt>},
	 {0x5ed0,     7, &_s<gt, byte_indirect>},
	 {0x5ed8,     7, &_s<gt, byte_postinc_indirect>},
	 {0x5ee0,     7, &_s<gt, byte_predec_indirect>},
	 {0x5ee8,     7, &_s<gt, byte_disp_indirect>},
	 {0x5ef0,     7, &_s<gt, byte_index_indirect>},
	 {0x5ef8,     0, &_s<gt, byte_abs_short>},
	 {0x5ef9,     0, &_s<gt, byte_abs_long>},
	 {0x5fc0,     7, &_s<le, byte_d_register>},
	 {0x5fc8,     7, &_db<le>},
	 {0x5fd0,     7, &_s<le, byte_indirect>},
	 {0x5fd8,     7, &_s<le, byte_postinc_indirect>},
	 {0x5fe0,     7, &_s<le, byte_predec_indirect>},
	 {0x5fe8,     7, &_s<le, byte_disp_indirect>},
	 {0x5ff0,     7, &_s<le, byte_index_indirect>},
	 {0x5ff8,     0, &_s<le, byte_abs_short>},
	 {0x5ff9,     0, &_s<le, byte_abs_long>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, d));
  }
}
