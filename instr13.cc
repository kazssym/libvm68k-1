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

#include <vm68k/addressing>
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
    using namespace addressing;

    /* Handles an ADD instruction (data register destination).  */
    template <class Size, class Source>
    uint32_type
    _add_d(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tadd%s %s,%%d%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get_s(c.regs.d[reg2]);
      typename Size::svalue_type value = Size::normal_s(value2 + value1);
      Size::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc_as_add(value, value2, value1);

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles an ADD instruction (memory destination).  */
    template <class Size, class Destination>
    uint32_type
    _add_m(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tadd%s %%d%u,%s\n", Size::suffix(), reg2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value2 = Size::get_s(c.regs.d[reg2]);
      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::normal_s(value1 + value2);
      ea1.put(c, value);
      c.regs.ccr.set_cc_as_add(value, value1, value2);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }

    /* Handles an ADDA instruction.  */
    template <class Size, class Source>
    uint32_type
    _adda(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tadda%s %s,%%a%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      // The condition codes are not affected by this instruction.
      long_word::svalue_type value1 = ea1.get(c);
      long_word::svalue_type value2 = long_word::get_s(c.regs.a[reg2]);
      long_word::svalue_type value = long_word::normal_s(value2 + value1);
      long_word::put(c.regs.a[reg2], value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles an ADDX instruction (data register).  */
    template <class Size>
    uint32_type
    _addx_d(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\taddx%s %%d%u,%%d%u\n", Size::suffix(), reg1, reg2);
#endif

      typename Size::svalue_type value1 = Size::get_s(c.regs.d[reg1]);
      typename Size::svalue_type value2 = Size::get_s(c.regs.d[reg2]);
      typename Size::svalue_type value
	= Size::normal_s(value2 + value1 + c.regs.ccr.x());
      Size::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc_as_add(value, value2, value1);

      return pc + 2;
    }
  }
  
  using namespace instr;

  void
  install_instructions_13(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0xd000, 0xe07, &_add_d<byte, byte_d_register>},
	 {0xd010, 0xe07, &_add_d<byte, byte_indirect>},
	 {0xd018, 0xe07, &_add_d<byte, byte_postinc_indirect>},
	 {0xd020, 0xe07, &_add_d<byte, byte_predec_indirect>},
	 {0xd028, 0xe07, &_add_d<byte, byte_disp_indirect>},
	 {0xd030, 0xe07, &_add_d<byte, byte_index_indirect>},
	 {0xd038, 0xe00, &_add_d<byte, byte_abs_short>},
	 {0xd039, 0xe00, &_add_d<byte, byte_abs_long>},
	 {0xd03a, 0xe00, &_add_d<byte, byte_disp_pc_indirect>},
	 {0xd03b, 0xe00, &_add_d<byte, byte_index_pc_indirect>},
	 {0xd03c, 0xe00, &_add_d<byte, byte_immediate>},
	 {0xd040, 0xe07, &_add_d<word, word_d_register>},
	 {0xd048, 0xe07, &_add_d<word, word_a_register>},
	 {0xd050, 0xe07, &_add_d<word, word_indirect>},
	 {0xd058, 0xe07, &_add_d<word, word_postinc_indirect>},
	 {0xd060, 0xe07, &_add_d<word, word_predec_indirect>},
	 {0xd068, 0xe07, &_add_d<word, word_disp_indirect>},
	 {0xd070, 0xe07, &_add_d<word, word_index_indirect>},
	 {0xd078, 0xe00, &_add_d<word, word_abs_short>},
	 {0xd079, 0xe00, &_add_d<word, word_abs_long>},
	 {0xd07a, 0xe00, &_add_d<word, word_disp_pc_indirect>},
	 {0xd07b, 0xe00, &_add_d<word, word_index_pc_indirect>},
	 {0xd07c, 0xe00, &_add_d<word, word_immediate>},
	 {0xd080, 0xe07, &_add_d<long_word, long_word_d_register>},
	 {0xd088, 0xe07, &_add_d<long_word, long_word_a_register>},
	 {0xd090, 0xe07, &_add_d<long_word, long_word_indirect>},
	 {0xd098, 0xe07, &_add_d<long_word, long_word_postinc_indirect>},
	 {0xd0a0, 0xe07, &_add_d<long_word, long_word_predec_indirect>},
	 {0xd0a8, 0xe07, &_add_d<long_word, long_word_disp_indirect>},
	 {0xd0b0, 0xe07, &_add_d<long_word, long_word_index_indirect>},
	 {0xd0b8, 0xe00, &_add_d<long_word, long_word_abs_short>},
	 {0xd0b9, 0xe00, &_add_d<long_word, long_word_abs_long>},
	 {0xd0ba, 0xe00, &_add_d<long_word, long_word_disp_pc_indirect>},
	 {0xd0bb, 0xe00, &_add_d<long_word, long_word_index_pc_indirect>},
	 {0xd0bc, 0xe00, &_add_d<long_word, long_word_immediate>},
	 {0xd0c0, 0xe07, &_adda<word, word_d_register>},
	 {0xd0c8, 0xe07, &_adda<word, word_a_register>},
	 {0xd0d0, 0xe07, &_adda<word, word_indirect>},
	 {0xd0d8, 0xe07, &_adda<word, word_postinc_indirect>},
	 {0xd0e0, 0xe07, &_adda<word, word_predec_indirect>},
	 {0xd0e8, 0xe07, &_adda<word, word_disp_indirect>},
	 {0xd0f0, 0xe07, &_adda<word, word_index_indirect>},
	 {0xd0f8, 0xe00, &_adda<word, word_abs_short>},
	 {0xd0f9, 0xe00, &_adda<word, word_abs_long>},
	 {0xd0fa, 0xe00, &_adda<word, word_disp_pc_indirect>},
	 {0xd0fb, 0xe00, &_adda<word, word_index_pc_indirect>},
	 {0xd0fc, 0xe00, &_adda<word, word_immediate>},
	 {0xd100, 0xe07, &_addx_d<byte>},
	 {0xd110, 0xe07, &_add_m<byte, byte_indirect>},
	 {0xd118, 0xe07, &_add_m<byte, byte_postinc_indirect>},
	 {0xd120, 0xe07, &_add_m<byte, byte_predec_indirect>},
	 {0xd128, 0xe07, &_add_m<byte, byte_disp_indirect>},
	 {0xd130, 0xe07, &_add_m<byte, byte_index_indirect>},
	 {0xd138, 0xe00, &_add_m<byte, byte_abs_short>},
	 {0xd139, 0xe00, &_add_m<byte, byte_abs_long>},
	 {0xd140, 0xe07, &_addx_d<word>},
	 {0xd150, 0xe07, &_add_m<word, word_indirect>},
	 {0xd158, 0xe07, &_add_m<word, word_postinc_indirect>},
	 {0xd160, 0xe07, &_add_m<word, word_predec_indirect>},
	 {0xd168, 0xe07, &_add_m<word, word_disp_indirect>},
	 {0xd170, 0xe07, &_add_m<word, word_index_indirect>},
	 {0xd178, 0xe00, &_add_m<word, word_abs_short>},
	 {0xd179, 0xe00, &_add_m<word, word_abs_long>},
	 {0xd180, 0xe07, &_addx_d<long_word>},
	 {0xd190, 0xe07, &_add_m<long_word, long_word_indirect>},
	 {0xd198, 0xe07, &_add_m<long_word, long_word_postinc_indirect>},
	 {0xd1a0, 0xe07, &_add_m<long_word, long_word_predec_indirect>},
	 {0xd1a8, 0xe07, &_add_m<long_word, long_word_disp_indirect>},
	 {0xd1b0, 0xe07, &_add_m<long_word, long_word_index_indirect>},
	 {0xd1b8, 0xe00, &_add_m<long_word, long_word_abs_short>},
	 {0xd1b9, 0xe00, &_add_m<long_word, long_word_abs_long>},
	 {0xd1c0, 0xe07, &_adda<long_word, long_word_d_register>},
	 {0xd1c8, 0xe07, &_adda<long_word, long_word_a_register>},
	 {0xd1d0, 0xe07, &_adda<long_word, long_word_indirect>},
	 {0xd1d8, 0xe07, &_adda<long_word, long_word_postinc_indirect>},
	 {0xd1e0, 0xe07, &_adda<long_word, long_word_predec_indirect>},
	 {0xd1e8, 0xe07, &_adda<long_word, long_word_disp_indirect>},
	 {0xd1f0, 0xe07, &_adda<long_word, long_word_index_indirect>},
	 {0xd1f8, 0xe00, &_adda<long_word, long_word_abs_short>},
	 {0xd1f9, 0xe00, &_adda<long_word, long_word_abs_long>},
	 {0xd1fa, 0xe00, &_adda<long_word, long_word_disp_pc_indirect>},
	 {0xd1fb, 0xe00, &_adda<long_word, long_word_index_pc_indirect>},
	 {0xd1fc, 0xe00, &_adda<long_word, long_word_immediate>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
