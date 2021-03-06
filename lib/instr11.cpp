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

    /* Handles a CMP instruction.  */
    template <class Size, class Source>
    uint32_type
    _cmp(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tcmp%s %s,%%d%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get_s(c.regs.d[reg2]);
      typename Size::svalue_type value = Size::normal_s(value2 - value1);
      c.regs.ccr.set_cc_cmp(value, value2, value1);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a CMPA instruction.  */
    template <class Size, class Source>
    uint32_type
    _cmpa(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tcmpa%s %s,%%a%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      long_word::svalue_type value1 = ea1.get(c);
      long_word::svalue_type value2 = long_word::get_s(c.regs.a[reg2]);
      long_word::svalue_type value
	= long_word::normal_s(value2 - value1);
      c.regs.ccr.set_cc_cmp(value, value2, value1);

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles a CMPM instruction.  */
    template <class Size>
    uint32_type
    _cmpm(uint32_type pc, context &c, uint16_type w, void *)
    {
      old_addressing<postinc_indirect, Size> ea1(w & 7, pc + 2);
      old_addressing<postinc_indirect, Size> ea2(w >> 9 & 7,
						 pc + 2 + ea1.extension_size());
#ifdef L
      L("\tcmpm%s %s,%s\n", Size::suffix(), ea1.text(c).c_str(),
	ea2.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = ea2.get(c);
      typename Size::svalue_type value = Size::normal_s(value2 - value1);
      c.regs.ccr.set_cc_cmp(value, value2, value1);

      ea1.finish(c);
      ea2.finish(c);
      return pc + 2 + ea1.extension_size() + ea2.extension_size();
    }

    /* Handles an EOR instruction (memory destination).  */
    template <class Size, class Destination>
    uint32_type
    _eor_m(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\teor%s %%d%u,%s\n", Size::suffix(), reg2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get_s(c.regs.d[reg2]);
      typename Size::svalue_type value
	= Size::normal_s(Size::normal_u(value1) ^ Size::normal_u(value2));
      ea1.put(c, value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }
  }
  
  using namespace instr;

  void
  install_instructions_11(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0xb000, 0xe07, &_cmp<byte, byte_d_register>},
	 {0xb010, 0xe07, &_cmp<byte, byte_indirect>},
	 {0xb018, 0xe07, &_cmp<byte, byte_postinc_indirect>},
	 {0xb020, 0xe07, &_cmp<byte, byte_predec_indirect>},
	 {0xb028, 0xe07, &_cmp<byte, byte_disp_indirect>},
	 {0xb030, 0xe07, &_cmp<byte, byte_index_indirect>},
	 {0xb038, 0xe00, &_cmp<byte, byte_abs_short>},
	 {0xb039, 0xe00, &_cmp<byte, byte_abs_long>},
	 {0xb03a, 0xe00, &_cmp<byte, byte_disp_pc_indirect>},
	 {0xb03b, 0xe00, &_cmp<byte, byte_index_pc_indirect>},
	 {0xb03c, 0xe00, &_cmp<byte, byte_immediate>},
	 {0xb040, 0xe07, &_cmp<word, word_d_register>},
	 {0xb048, 0xe07, &_cmp<word, word_a_register>},
	 {0xb050, 0xe07, &_cmp<word, word_indirect>},
	 {0xb058, 0xe07, &_cmp<word, word_postinc_indirect>},
	 {0xb060, 0xe07, &_cmp<word, word_predec_indirect>},
	 {0xb068, 0xe07, &_cmp<word, word_disp_indirect>},
	 {0xb070, 0xe07, &_cmp<word, word_index_indirect>},
	 {0xb078, 0xe00, &_cmp<word, word_abs_short>},
	 {0xb079, 0xe00, &_cmp<word, word_abs_long>},
	 {0xb07a, 0xe00, &_cmp<word, word_disp_pc_indirect>},
	 {0xb07b, 0xe00, &_cmp<word, word_index_pc_indirect>},
	 {0xb07c, 0xe00, &_cmp<word, word_immediate>},
	 {0xb080, 0xe07, &_cmp<long_word, long_word_d_register>},
	 {0xb088, 0xe07, &_cmp<long_word, long_word_a_register>},
	 {0xb090, 0xe07, &_cmp<long_word, long_word_indirect>},
	 {0xb098, 0xe07, &_cmp<long_word, long_word_postinc_indirect>},
	 {0xb0a0, 0xe07, &_cmp<long_word, long_word_predec_indirect>},
	 {0xb0a8, 0xe07, &_cmp<long_word, long_word_disp_indirect>},
	 {0xb0b0, 0xe07, &_cmp<long_word, long_word_index_indirect>},
	 {0xb0b8, 0xe00, &_cmp<long_word, long_word_abs_short>},
	 {0xb0b9, 0xe00, &_cmp<long_word, long_word_abs_long>},
	 {0xb0ba, 0xe00, &_cmp<long_word, long_word_disp_pc_indirect>},
	 {0xb0bb, 0xe00, &_cmp<long_word, long_word_index_pc_indirect>},
	 {0xb0bc, 0xe00, &_cmp<long_word, long_word_immediate>},
	 {0xb0c0, 0xe07, &_cmpa<word, word_d_register>},
	 {0xb0c8, 0xe07, &_cmpa<word, word_a_register>},
	 {0xb0d0, 0xe07, &_cmpa<word, word_indirect>},
	 {0xb0d8, 0xe07, &_cmpa<word, word_postinc_indirect>},
	 {0xb0e0, 0xe07, &_cmpa<word, word_predec_indirect>},
	 {0xb0e8, 0xe07, &_cmpa<word, word_disp_indirect>},
	 {0xb0f0, 0xe07, &_cmpa<word, word_index_indirect>},  
	 {0xb0f8, 0xe00, &_cmpa<word, word_abs_short>},
	 {0xb0f9, 0xe00, &_cmpa<word, word_abs_long>},
	 {0xb0fa, 0xe00, &_cmpa<word, word_disp_pc_indirect>},
	 {0xb0fb, 0xe00, &_cmpa<word, word_index_pc_indirect>},
	 {0xb0fc, 0xe00, &_cmpa<word, word_immediate>},
	 {0xb100, 0xe07, &_eor_m<byte, byte_d_register>},
	 {0xb108, 0xe07, &_cmpm<byte>},
	 {0xb110, 0xe07, &_eor_m<byte, byte_indirect>},
	 {0xb118, 0xe07, &_eor_m<byte, byte_postinc_indirect>},
	 {0xb120, 0xe07, &_eor_m<byte, byte_predec_indirect>},
	 {0xb128, 0xe07, &_eor_m<byte, byte_disp_indirect>},
	 {0xb130, 0xe07, &_eor_m<byte, byte_index_indirect>},
	 {0xb138, 0xe00, &_eor_m<byte, byte_abs_short>},
	 {0xb139, 0xe00, &_eor_m<byte, byte_abs_long>},
	 {0xb140, 0xe07, &_eor_m<word, word_d_register>},
	 {0xb148, 0xe07, &_cmpm<word>},
	 {0xb150, 0xe07, &_eor_m<word, word_indirect>},
	 {0xb158, 0xe07, &_eor_m<word, word_postinc_indirect>},
	 {0xb160, 0xe07, &_eor_m<word, word_predec_indirect>},
	 {0xb168, 0xe07, &_eor_m<word, word_disp_indirect>},
	 {0xb170, 0xe07, &_eor_m<word, word_index_indirect>},
	 {0xb178, 0xe00, &_eor_m<word, word_abs_short>},
	 {0xb179, 0xe00, &_eor_m<word, word_abs_long>},
	 {0xb180, 0xe07, &_eor_m<long_word, long_word_d_register>},
	 {0xb188, 0xe07, &_cmpm<long_word>},
	 {0xb190, 0xe07, &_eor_m<long_word, long_word_indirect>},
	 {0xb198, 0xe07, &_eor_m<long_word, long_word_postinc_indirect>},
	 {0xb1a0, 0xe07, &_eor_m<long_word, long_word_predec_indirect>},
	 {0xb1a8, 0xe07, &_eor_m<long_word, long_word_disp_indirect>},
	 {0xb1b0, 0xe07, &_eor_m<long_word, long_word_index_indirect>},
	 {0xb1b8, 0xe00, &_eor_m<long_word, long_word_abs_short>},
	 {0xb1b9, 0xe00, &_eor_m<long_word, long_word_abs_long>},
	 {0xb1c0, 0xe07, &_cmpa<long_word, long_word_d_register>},
	 {0xb1c8, 0xe07, &_cmpa<long_word, long_word_a_register>},
	 {0xb1d0, 0xe07, &_cmpa<long_word, long_word_indirect>},
	 {0xb1d8, 0xe07, &_cmpa<long_word, long_word_postinc_indirect>},
	 {0xb1e0, 0xe07, &_cmpa<long_word, long_word_predec_indirect>},
	 {0xb1e8, 0xe07, &_cmpa<long_word, long_word_disp_indirect>},
	 {0xb1f0, 0xe07, &_cmpa<long_word, long_word_index_indirect>},
	 {0xb1f8, 0xe00, &_cmpa<long_word, long_word_abs_short>},
	 {0xb1f9, 0xe00, &_cmpa<long_word, long_word_abs_long>},
	 {0xb1fa, 0xe00, &_cmpa<long_word, long_word_disp_pc_indirect>},
	 {0xb1fb, 0xe00, &_cmpa<long_word, long_word_index_pc_indirect>},
	 {0xb1fc, 0xe00, &_cmpa<long_word, long_word_immediate>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
