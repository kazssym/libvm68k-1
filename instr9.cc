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

#include <vm68k/add.h>
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
    using namespace add;

    /* Handles a SUB instruction (data register destination).  */
    template <class Size, class Source>
    uint32_type
    _sub_d(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tsub%s %s,%%d%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value = Size::svalue(value2 - value1);
      Size::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc_sub(value, value2, value1);

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles a SUB instruction (memory destination).  */
    template <class Size, class Destination>
    uint32_type
    _sub_m(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tsub%s %%d%u,%s\n", Size::suffix(), reg2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::svalue(value1 - value2);
      ea1.put(c, value);
      c.regs.ccr.set_cc_sub(value, value1, value2);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a SUBA instruction.  */
    template <class Size, class Source>
    uint32_type
    _suba(uint32_type pc, context &c, uint16_type w, unsigned long)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tsuba%s %s,%%a%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      // This instruction does not affect the condition codes.
      long_word::svalue_type value1 = ea1.get(c);
      long_word::svalue_type value2 = long_word::get(c.regs.a[reg2]);
      long_word::svalue_type value
	= long_word::svalue(value2 - value1);
      long_word::put(c.regs.a[reg2], value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }
  }
  
  using namespace instr;

  void
  install_instructions_9(processor &p, unsigned long d)
  {
    static const instruction_map inst[]
      = {{0x9000, 0xe07, &_sub_d<byte, byte_d_register>},
	 {0x9010, 0xe07, &_sub_d<byte, byte_indirect>},
	 {0x9018, 0xe07, &_sub_d<byte, byte_postinc_indirect>},
	 {0x9020, 0xe07, &_sub_d<byte, byte_predec_indirect>},
	 {0x9028, 0xe07, &_sub_d<byte, byte_disp_indirect>},
	 {0x9030, 0xe07, &_sub_d<byte, byte_index_indirect>},
	 {0x9038, 0xe00, &_sub_d<byte, byte_abs_short>},
	 {0x9039, 0xe00, &_sub_d<byte, byte_abs_long>},
	 {0x903a, 0xe00, &_sub_d<byte, byte_disp_pc_indirect>},
	 {0x903b, 0xe00, &_sub_d<byte, byte_index_pc_indirect>},
	 {0x903c, 0xe00, &_sub_d<byte, byte_immediate>},
	 {0x9040, 0xe07, &_sub_d<word, word_d_register>},
	 {0x9048, 0xe07, &_sub_d<word, word_a_register>},
	 {0x9050, 0xe07, &_sub_d<word, word_indirect>},
	 {0x9058, 0xe07, &_sub_d<word, word_postinc_indirect>},
	 {0x9060, 0xe07, &_sub_d<word, word_predec_indirect>},
	 {0x9068, 0xe07, &_sub_d<word, word_disp_indirect>},
	 {0x9070, 0xe07, &_sub_d<word, word_index_indirect>},
	 {0x9078, 0xe00, &_sub_d<word, word_abs_short>},
	 {0x9079, 0xe00, &_sub_d<word, word_abs_long>},
	 {0x907a, 0xe00, &_sub_d<word, word_disp_pc_indirect>},
	 {0x907b, 0xe00, &_sub_d<word, word_index_pc_indirect>},
	 {0x907c, 0xe00, &_sub_d<word, word_immediate>},
	 {0x9080, 0xe07, &_sub_d<long_word, long_word_d_register>},
	 {0x9088, 0xe07, &_sub_d<long_word, long_word_a_register>},
	 {0x9090, 0xe07, &_sub_d<long_word, long_word_indirect>},
	 {0x9098, 0xe07, &_sub_d<long_word, long_word_postinc_indirect>},
	 {0x90a0, 0xe07, &_sub_d<long_word, long_word_predec_indirect>},
	 {0x90a8, 0xe07, &_sub_d<long_word, long_word_disp_indirect>},
	 {0x90b0, 0xe07, &_sub_d<long_word, long_word_index_indirect>},
	 {0x90b8, 0xe00, &_sub_d<long_word, long_word_abs_short>},
	 {0x90b9, 0xe00, &_sub_d<long_word, long_word_abs_long>},
	 {0x90ba, 0xe00, &_sub_d<long_word, long_word_disp_pc_indirect>},
	 {0x90bb, 0xe00, &_sub_d<long_word, long_word_index_pc_indirect>},
	 {0x90bc, 0xe00, &_sub_d<long_word, long_word_immediate>},
	 {0x90c0, 0xe07, &_suba<word, word_d_register>},
	 {0x90c8, 0xe07, &_suba<word, word_a_register>},
	 {0x90d0, 0xe07, &_suba<word, word_indirect>},
	 {0x90d8, 0xe07, &_suba<word, word_postinc_indirect>},
	 {0x90e0, 0xe07, &_suba<word, word_predec_indirect>},
	 {0x90e8, 0xe07, &_suba<word, word_disp_indirect>},
	 {0x90f0, 0xe07, &_suba<word, word_index_indirect>},
	 {0x90f8, 0xe00, &_suba<word, word_abs_short>},
	 {0x90f9, 0xe00, &_suba<word, word_abs_long>},
	 {0x90fa, 0xe00, &_suba<word, word_disp_pc_indirect>},
	 {0x90fb, 0xe00, &_suba<word, word_index_pc_indirect>},
	 {0x90fc, 0xe00, &_suba<word, word_immediate>},
	 {0x9110, 0xe07, &_sub_m<byte, byte_indirect>},
	 {0x9118, 0xe07, &_sub_m<byte, byte_postinc_indirect>},
	 {0x9120, 0xe07, &_sub_m<byte, byte_predec_indirect>},
	 {0x9128, 0xe07, &_sub_m<byte, byte_disp_indirect>},
	 {0x9130, 0xe07, &_sub_m<byte, byte_index_indirect>},
	 {0x9138, 0xe00, &_sub_m<byte, byte_abs_short>},
	 {0x9139, 0xe00, &_sub_m<byte, byte_abs_long>},
	 {0x9150, 0xe07, &_sub_m<word, word_indirect>},
	 {0x9158, 0xe07, &_sub_m<word, word_postinc_indirect>},
	 {0x9160, 0xe07, &_sub_m<word, word_predec_indirect>},
	 {0x9168, 0xe07, &_sub_m<word, word_disp_indirect>},
	 {0x9170, 0xe07, &_sub_m<word, word_index_indirect>},
	 {0x9178, 0xe00, &_sub_m<word, word_abs_short>},
	 {0x9179, 0xe00, &_sub_m<word, word_abs_long>},
	 {0x9190, 0xe07, &_sub_m<long_word, long_word_indirect>},
	 {0x9198, 0xe07, &_sub_m<long_word, long_word_postinc_indirect>},
	 {0x91a0, 0xe07, &_sub_m<long_word, long_word_predec_indirect>},
	 {0x91a8, 0xe07, &_sub_m<long_word, long_word_disp_indirect>},
	 {0x91b0, 0xe07, &_sub_m<long_word, long_word_index_indirect>},
	 {0x91b8, 0xe00, &_sub_m<long_word, long_word_abs_short>},
	 {0x91b9, 0xe00, &_sub_m<long_word, long_word_abs_long>},
	 {0x91c0, 0xe07, &_suba<long_word, long_word_d_register>},
	 {0x91c8, 0xe07, &_suba<long_word, long_word_a_register>},
	 {0x91d0, 0xe07, &_suba<long_word, long_word_indirect>},
	 {0x91d8, 0xe07, &_suba<long_word, long_word_postinc_indirect>},
	 {0x91e0, 0xe07, &_suba<long_word, long_word_predec_indirect>},
	 {0x91e8, 0xe07, &_suba<long_word, long_word_disp_indirect>},
	 {0x91f0, 0xe07, &_suba<long_word, long_word_index_indirect>},
	 {0x91f8, 0xe00, &_suba<long_word, long_word_abs_short>},
	 {0x91f9, 0xe00, &_suba<long_word, long_word_abs_long>},
	 {0x91fa, 0xe00, &_suba<long_word, long_word_disp_pc_indirect>},
	 {0x91fb, 0xe00, &_suba<long_word, long_word_index_pc_indirect>},
	 {0x91fc, 0xe00, &_suba<long_word, long_word_immediate>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, d));
  }
}
