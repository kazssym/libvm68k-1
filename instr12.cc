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

    /* Handles an AND instruction (data register destination).  */
    template <class Size, class Source>
    uint32_type
    _and_d(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tand%s %s,%%d%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value
	= Size::svalue(Size::uvalue(value2) & Size::uvalue(value1));
      Size::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles an AND instruction (memory destination).  */
    template <class Size, class Destination>
    uint32_type
    _and_m(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tand%s %%d%u,%s\n", Size::suffix(), reg2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value
	= Size::svalue(Size::uvalue(value1) & Size::uvalue(value2));
      ea1.put(c, value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles an EXG instruction (data registers).  */
    uint32_type
    _exg_d_d(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\texg%s %%d%u,%%d%u\n", long_word::suffix(), reg2, reg1);
#endif

      // The condition codes are not affected by this instruction.
      long_word::svalue_type value
	= long_word::get(c.regs.d[reg1]);
      long_word::put(c.regs.d[reg1], long_word::get(c.regs.d[reg2]));
      long_word::put(c.regs.d[reg2], value);

      return pc + 2;
    }

    /* Handles an EXG instruction (address registers).  */
    uint32_type
    _exg_a_a(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\texg%s %%a%u,%%a%u\n", long_word::suffix(), reg2, reg1);
#endif

      // The condition codes are not affected by this instruction.
      long_word::svalue_type value
	= long_word::get(c.regs.a[reg1]);
      long_word::put(c.regs.a[reg1], long_word::get(c.regs.a[reg2]));
      long_word::put(c.regs.a[reg2], value);

      return pc + 2;
    }

    /* Handles an EXG instruction (data register and address register).  */
    uint32_type
    _exg_d_a(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\texg%s %%d%u,%%a%u\n", long_word::suffix(), reg2, reg1);
#endif

      // The condition codes are not affected by this instruction.
      long_word::svalue_type value
	= long_word::get(c.regs.a[reg1]);
      long_word::put(c.regs.a[reg1], long_word::get(c.regs.d[reg2]));
      long_word::put(c.regs.d[reg2], value);

      return pc + 2;
    }

    /* Handles a MULS instruction.  */
    template <class Source>
    uint32_type
    _muls(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tmuls%s %s,%%d%u\n", word::suffix(), ea1.text(c).c_str(), reg2);
#endif

      word::svalue_type value1 = ea1.get(c);
      word::svalue_type value2 = word::get(c.regs.d[reg2]);
      long_word::svalue_type value
	= (long_word::svalue_type(value2)
	   * long_word::svalue_type(value1));
      long_word::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc(value);	// FIXME.

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles a MULU instruction.  */
    template <class Source>
    uint32_type
    _mulu(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tmulu%s %s,%%d%u\n", word::suffix(), ea1.text(c).c_str(), reg2);
#endif

      word::svalue_type value1 = ea1.get(c);
      word::svalue_type value2 = word::get(c.regs.d[reg2]);
      long_word::svalue_type value
	= (long_word::svalue
	   (long_word::uvalue(word::uvalue(value2))
	    * long_word::uvalue(word::uvalue(value1))));
      long_word::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc(value); // FIXME.

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }
  }
  
  using namespace instr;

  void
  install_instructions_12(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0xc000, 0xe07, &_and_d<byte, byte_d_register>},
	 {0xc010, 0xe07, &_and_d<byte, byte_indirect>},
	 {0xc018, 0xe07, &_and_d<byte, byte_postinc_indirect>},
	 {0xc020, 0xe07, &_and_d<byte, byte_predec_indirect>},
	 {0xc028, 0xe07, &_and_d<byte, byte_disp_indirect>},
	 {0xc030, 0xe07, &_and_d<byte, byte_index_indirect>},
	 {0xc038, 0xe00, &_and_d<byte, byte_abs_short>},
	 {0xc039, 0xe00, &_and_d<byte, byte_abs_long>},
	 {0xc03a, 0xe00, &_and_d<byte, byte_disp_pc_indirect>},
	 {0xc03b, 0xe00, &_and_d<byte, byte_index_pc_indirect>},
	 {0xc03c, 0xe00, &_and_d<byte, byte_immediate>},
	 {0xc040, 0xe07, &_and_d<word, word_d_register>},
	 {0xc050, 0xe07, &_and_d<word, word_indirect>},
	 {0xc058, 0xe07, &_and_d<word, word_postinc_indirect>},
	 {0xc060, 0xe07, &_and_d<word, word_predec_indirect>},
	 {0xc068, 0xe07, &_and_d<word, word_disp_indirect>},
	 {0xc070, 0xe07, &_and_d<word, word_index_indirect>},
	 {0xc078, 0xe00, &_and_d<word, word_abs_short>},
	 {0xc079, 0xe00, &_and_d<word, word_abs_long>},
	 {0xc07a, 0xe00, &_and_d<word, word_disp_pc_indirect>},
	 {0xc07b, 0xe00, &_and_d<word, word_index_pc_indirect>},
	 {0xc07c, 0xe00, &_and_d<word, word_immediate>},
	 {0xc080, 0xe07, &_and_d<long_word, long_word_d_register>},
	 {0xc090, 0xe07, &_and_d<long_word, long_word_indirect>},
	 {0xc098, 0xe07, &_and_d<long_word, long_word_postinc_indirect>},
	 {0xc0a0, 0xe07, &_and_d<long_word, long_word_predec_indirect>},
	 {0xc0a8, 0xe07, &_and_d<long_word, long_word_disp_indirect>},
	 {0xc0b0, 0xe07, &_and_d<long_word, long_word_index_indirect>},
	 {0xc0b8, 0xe00, &_and_d<long_word, long_word_abs_short>},
	 {0xc0b9, 0xe00, &_and_d<long_word, long_word_abs_long>},
	 {0xc0ba, 0xe00, &_and_d<long_word, long_word_disp_pc_indirect>},
	 {0xc0bb, 0xe00, &_and_d<long_word, long_word_index_pc_indirect>},
	 {0xc0bc, 0xe00, &_and_d<long_word, long_word_immediate>},
	 {0xc0c0, 0xe07, &_mulu<word_d_register>},
	 {0xc0d0, 0xe07, &_mulu<word_indirect>},
	 {0xc0d8, 0xe07, &_mulu<word_postinc_indirect>},
	 {0xc0e0, 0xe07, &_mulu<word_predec_indirect>},
	 {0xc0e8, 0xe07, &_mulu<word_disp_indirect>},
	 {0xc0f0, 0xe07, &_mulu<word_index_indirect>},
	 {0xc0f8, 0xe00, &_mulu<word_abs_short>},
	 {0xc0f9, 0xe00, &_mulu<word_abs_long>},
	 {0xc0fa, 0xe00, &_mulu<word_disp_pc_indirect>},
	 {0xc0fb, 0xe00, &_mulu<word_index_pc_indirect>},
	 {0xc0fc, 0xe00, &_mulu<word_immediate>},
	 {0xc110, 0xe07, &_and_m<byte, byte_indirect>},
	 {0xc118, 0xe07, &_and_m<byte, byte_postinc_indirect>},
	 {0xc120, 0xe07, &_and_m<byte, byte_predec_indirect>},
	 {0xc128, 0xe07, &_and_m<byte, byte_disp_indirect>},
	 {0xc130, 0xe07, &_and_m<byte, byte_index_indirect>},
	 {0xc138, 0xe00, &_and_m<byte, byte_abs_short>},
	 {0xc139, 0xe00, &_and_m<byte, byte_abs_long>},
	 {0xc140, 0xe07, &_exg_d_d},
	 {0xc148, 0xe07, &_exg_a_a},
	 {0xc150, 0xe07, &_and_m<word, word_indirect>},
	 {0xc158, 0xe07, &_and_m<word, word_postinc_indirect>},
	 {0xc160, 0xe07, &_and_m<word, word_predec_indirect>},
	 {0xc168, 0xe07, &_and_m<word, word_disp_indirect>},
	 {0xc170, 0xe07, &_and_m<word, word_index_indirect>},
	 {0xc178, 0xe00, &_and_m<word, word_abs_short>},
	 {0xc179, 0xe00, &_and_m<word, word_abs_long>},
	 {0xc188, 0xe07, &_exg_d_a},
	 {0xc190, 0xe07, &_and_m<long_word, long_word_indirect>},
	 {0xc198, 0xe07, &_and_m<long_word, long_word_postinc_indirect>},
	 {0xc1a0, 0xe07, &_and_m<long_word, long_word_predec_indirect>},
	 {0xc1a8, 0xe07, &_and_m<long_word, long_word_disp_indirect>},
	 {0xc1b0, 0xe07, &_and_m<long_word, long_word_index_indirect>},
	 {0xc1b8, 0xe00, &_and_m<long_word, long_word_abs_short>},
	 {0xc1b9, 0xe00, &_and_m<long_word, long_word_abs_long>},
	 {0xc1c0, 0xe07, &_muls<word_d_register>},
	 {0xc1d0, 0xe07, &_muls<word_indirect>},
	 {0xc1d8, 0xe07, &_muls<word_postinc_indirect>},
	 {0xc1e0, 0xe07, &_muls<word_predec_indirect>},
	 {0xc1e8, 0xe07, &_muls<word_disp_indirect>},
	 {0xc1f0, 0xe07, &_muls<word_index_indirect>},
	 {0xc1f8, 0xe00, &_muls<word_abs_short>},
	 {0xc1f9, 0xe00, &_muls<word_abs_long>},
	 {0xc1fa, 0xe00, &_muls<word_disp_pc_indirect>},
	 {0xc1fb, 0xe00, &_muls<word_index_pc_indirect>},
	 {0xc1fc, 0xe00, &_muls<word_immediate>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
