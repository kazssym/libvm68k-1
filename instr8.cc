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

    /* Handles a DIVU instruction.  */
    template <class Source>
    uint32_type
    _divu(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tdivu%s %s,%%d%u\n", word::suffix(), ea1.text(c).c_str(), reg2);
#endif

      word::svalue_type value1 = ea1.get(c);
      long_word::svalue_type value2 = long_word::get(c.regs.d[reg2]);
      long_word::svalue_type value
	= long_word::uvalue(value2) / word::uvalue(value1);
      long_word::svalue_type rem
	= long_word::uvalue(value2) % word::uvalue(value1);
      long_word::put(c.regs.d[reg2],
			  long_word::uvalue(rem) << 16
			  | long_word::uvalue(value) & 0xffff);
      c.regs.ccr.set_cc(value);	// FIXME.

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles an OR instruction (data register destination).  */
    template <class Size, class Source>
    uint32_type
    _or_d(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tor%s %s,%%d%u\n", Size::suffix(), ea1.text(c).c_str(), reg2);
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value
	= Size::svalue(Size::uvalue(value2) | Size::uvalue(value1));
      Size::put(c.regs.d[reg2], value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + Source::extension_size();
    }

    /* Handles an OR instruction (memory destination).  */
    template <class Size, class Destination>
    uint32_type
    _or_m(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tor%s %%d%u,%s\n", Size::suffix(), reg2, ea1.text(c).c_str());
#endif

      typename Size::svalue_type value2 = Size::get(c.regs.d[reg2]);
      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value
	= Size::svalue(Size::uvalue(value1) | Size::uvalue(value2));
      ea1.put(c, value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }
  }
  
  using namespace instr;

  void
  install_instructions_8(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x8000, 0xe07, &_or_d<byte, byte_d_register>},
	 {0x8010, 0xe07, &_or_d<byte, byte_indirect>},
	 {0x8018, 0xe07, &_or_d<byte, byte_postinc_indirect>},
	 {0x8020, 0xe07, &_or_d<byte, byte_predec_indirect>},
	 {0x8028, 0xe07, &_or_d<byte, byte_disp_indirect>},
	 {0x8030, 0xe07, &_or_d<byte, byte_index_indirect>},
	 {0x8038, 0xe00, &_or_d<byte, byte_abs_short>},
	 {0x8039, 0xe00, &_or_d<byte, byte_abs_long>},
	 {0x803a, 0xe00, &_or_d<byte, byte_disp_pc_indirect>},
	 {0x803b, 0xe00, &_or_d<byte, byte_index_pc_indirect>},
	 {0x803c, 0xe00, &_or_d<byte, byte_immediate>},
	 {0x8040, 0xe07, &_or_d<word, word_d_register>},
	 {0x8050, 0xe07, &_or_d<word, word_indirect>},
	 {0x8058, 0xe07, &_or_d<word, word_postinc_indirect>},
	 {0x8060, 0xe07, &_or_d<word, word_predec_indirect>},
	 {0x8068, 0xe07, &_or_d<word, word_disp_indirect>},
	 {0x8070, 0xe07, &_or_d<word, word_index_indirect>},
	 {0x8078, 0xe00, &_or_d<word, word_abs_short>},
	 {0x8079, 0xe00, &_or_d<word, word_abs_long>},
	 {0x807a, 0xe00, &_or_d<word, word_disp_pc_indirect>},
	 {0x807b, 0xe00, &_or_d<word, word_index_pc_indirect>},
	 {0x807c, 0xe00, &_or_d<word, word_immediate>},
	 {0x8080, 0xe07, &_or_d<long_word, long_word_d_register>},
	 {0x8090, 0xe07, &_or_d<long_word, long_word_indirect>},
	 {0x8098, 0xe07, &_or_d<long_word, long_word_postinc_indirect>},
	 {0x80a0, 0xe07, &_or_d<long_word, long_word_predec_indirect>},
	 {0x80a8, 0xe07, &_or_d<long_word, long_word_disp_indirect>},
	 {0x80b0, 0xe07, &_or_d<long_word, long_word_index_indirect>},
	 {0x80b8, 0xe00, &_or_d<long_word, long_word_abs_short>},
	 {0x80b9, 0xe00, &_or_d<long_word, long_word_abs_long>},
	 {0x80ba, 0xe00, &_or_d<long_word, long_word_disp_pc_indirect>},
	 {0x80bb, 0xe00, &_or_d<long_word, long_word_index_pc_indirect>},
	 {0x80bc, 0xe00, &_or_d<long_word, long_word_immediate>},
	 {0x80c0, 0xe07, &_divu<word_d_register>},
	 {0x80d0, 0xe07, &_divu<word_indirect>},
	 {0x80d8, 0xe07, &_divu<word_postinc_indirect>},
	 {0x80e0, 0xe07, &_divu<word_predec_indirect>},
	 {0x80e8, 0xe07, &_divu<word_disp_indirect>},
	 {0x80f0, 0xe07, &_divu<word_index_indirect>},
	 {0x80f8, 0xe00, &_divu<word_abs_short>},
	 {0x80f9, 0xe00, &_divu<word_abs_long>},
	 {0x80fa, 0xe00, &_divu<word_disp_pc_indirect>},
	 {0x80fb, 0xe00, &_divu<word_index_pc_indirect>},
	 {0x80fc, 0xe00, &_divu<word_immediate>},
	 {0x8110, 0xe07, &_or_m<byte, byte_indirect>},
	 {0x8118, 0xe07, &_or_m<byte, byte_postinc_indirect>},
	 {0x8120, 0xe07, &_or_m<byte, byte_predec_indirect>},
	 {0x8128, 0xe07, &_or_m<byte, byte_disp_indirect>},
	 {0x8130, 0xe07, &_or_m<byte, byte_index_indirect>},
	 {0x8138, 0xe00, &_or_m<byte, byte_abs_short>},
	 {0x8139, 0xe00, &_or_m<byte, byte_abs_long>},
	 {0x8150, 0xe07, &_or_m<word, word_indirect>},
	 {0x8158, 0xe07, &_or_m<word, word_postinc_indirect>},
	 {0x8160, 0xe07, &_or_m<word, word_predec_indirect>},
	 {0x8168, 0xe07, &_or_m<word, word_disp_indirect>},
	 {0x8170, 0xe07, &_or_m<word, word_index_indirect>},
	 {0x8178, 0xe00, &_or_m<word, word_abs_short>},
	 {0x8179, 0xe00, &_or_m<word, word_abs_long>},
	 {0x8190, 0xe07, &_or_m<long_word, long_word_indirect>},
	 {0x8198, 0xe07, &_or_m<long_word, long_word_postinc_indirect>},
	 {0x81a0, 0xe07, &_or_m<long_word, long_word_predec_indirect>},
	 {0x81a8, 0xe07, &_or_m<long_word, long_word_disp_indirect>},
	 {0x81b0, 0xe07, &_or_m<long_word, long_word_index_indirect>},
	 {0x81b8, 0xe00, &_or_m<long_word, long_word_abs_short>},
	 {0x81b9, 0xe00, &_or_m<long_word, long_word_abs_long>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
