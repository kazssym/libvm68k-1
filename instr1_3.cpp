/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2002 Hypercore Software Design, Ltd.

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

#define _GNU_SOURCE 1
#define _POSIX_C_SOURCE 199506L	// POSIX.1c

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

    /* Handles a MOVE instruction.  */
    template <class Size, class Source, class Destination>
    uint32_type
    m68k_move(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      Destination ea2(w >> 9 & 7, pc + 2 + ea1.extension_size(Size()));
#ifdef L
      L("\tmove%s %s,%s\n", Size::suffix(), ea1.text(Size(), c).c_str(),
	ea2.text(Size(), c).c_str());
#endif

      typename Size::svalue_type value = ea1.get_s(Size(), c);
      ea2.put(Size(), c, value);
      c.regs.ccr.set_cc(value);

      ea1.finish(Size(), c);
      ea2.finish(Size(), c);
      return pc + 2 + ea1.extension_size(Size()) + ea2.extension_size(Size());
    }

    /* Handles a MOVEA instruction.  */
    template <class Size, class Source>
    uint32_type
    m68k_movea(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tmovea%s %s,%%a%u\n", Size::suffix(), ea1.text(Size(), c).c_str(),
	reg2);
#endif

      // The condition codes are not affected by this instruction.
      long_word::svalue_type value = ea1.get_s(Size(), c);
      long_word::put(c.regs.a[reg2], value);

      ea1.finish(Size(), c);
      return pc + 2 + ea1.extension_size(Size());
    }
  }

  using namespace instr;

  void
  install_instructions_1(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x1000, 0xe07, &m68k_move<byte, data_register_direct, data_register_direct>},
	 {0x1010, 0xe07, &m68k_move<byte, indirect, data_register_direct>},
	 {0x1018, 0xe07, &m68k_move<byte, postinc_indirect, data_register_direct>},
	 {0x1020, 0xe07, &m68k_move<byte, predec_indirect, data_register_direct>},
	 {0x1028, 0xe07, &m68k_move<byte, disp_indirect, data_register_direct>},
	 {0x1030, 0xe07, &m68k_move<byte, index_indirect, data_register_direct>},
	 {0x1038, 0xe00, &m68k_move<byte, absolute_short, data_register_direct>},
	 {0x1039, 0xe00, &m68k_move<byte, absolute_long, data_register_direct>},
	 {0x103a, 0xe00, &m68k_move<byte, disp_pc_indirect, data_register_direct>},
	 {0x103b, 0xe00, &m68k_move<byte, index_pc_indirect, data_register_direct>},
	 {0x103c, 0xe00, &m68k_move<byte, immediate, data_register_direct>},
	 {0x1080, 0xe07, &m68k_move<byte, data_register_direct, indirect>},
	 {0x1090, 0xe07, &m68k_move<byte, indirect, indirect>},
	 {0x1098, 0xe07, &m68k_move<byte, postinc_indirect, indirect>},
	 {0x10a0, 0xe07, &m68k_move<byte, predec_indirect, indirect>},
	 {0x10a8, 0xe07, &m68k_move<byte, disp_indirect, indirect>},
	 {0x10b0, 0xe07, &m68k_move<byte, index_indirect, indirect>},
	 {0x10b8, 0xe00, &m68k_move<byte, absolute_short, indirect>},
	 {0x10b9, 0xe00, &m68k_move<byte, absolute_long, indirect>},
	 {0x10ba, 0xe00, &m68k_move<byte, disp_pc_indirect, indirect>},
	 {0x10bb, 0xe00, &m68k_move<byte, index_pc_indirect, indirect>},
	 {0x10bc, 0xe00, &m68k_move<byte, immediate, indirect>},
	 {0x10c0, 0xe07, &m68k_move<byte, data_register_direct, postinc_indirect>},
	 {0x10d0, 0xe07, &m68k_move<byte, indirect, postinc_indirect>},
	 {0x10d8, 0xe07, &m68k_move<byte, postinc_indirect, postinc_indirect>},
	 {0x10e0, 0xe07, &m68k_move<byte, predec_indirect, postinc_indirect>},
	 {0x10e8, 0xe07, &m68k_move<byte, disp_indirect, postinc_indirect>},
	 {0x10f0, 0xe07, &m68k_move<byte, index_indirect, postinc_indirect>},
	 {0x10f8, 0xe00, &m68k_move<byte, absolute_short, postinc_indirect>},
	 {0x10f9, 0xe00, &m68k_move<byte, absolute_long, postinc_indirect>},
	 {0x10fa, 0xe00, &m68k_move<byte, disp_pc_indirect, postinc_indirect>},
	 {0x10fb, 0xe00, &m68k_move<byte, index_pc_indirect, postinc_indirect>},
	 {0x10fc, 0xe00, &m68k_move<byte, immediate, postinc_indirect>},
	 {0x1100, 0xe07, &m68k_move<byte, data_register_direct, predec_indirect>},
	 {0x1110, 0xe07, &m68k_move<byte, indirect, predec_indirect>},
	 {0x1118, 0xe07, &m68k_move<byte, postinc_indirect, predec_indirect>},
	 {0x1120, 0xe07, &m68k_move<byte, predec_indirect, predec_indirect>},
	 {0x1128, 0xe07, &m68k_move<byte, disp_indirect, predec_indirect>},
	 {0x1130, 0xe07, &m68k_move<byte, index_indirect, predec_indirect>},
	 {0x1138, 0xe00, &m68k_move<byte, absolute_short, predec_indirect>},
	 {0x1139, 0xe00, &m68k_move<byte, absolute_long, predec_indirect>},
	 {0x113a, 0xe00, &m68k_move<byte, disp_pc_indirect, predec_indirect>},
	 {0x113b, 0xe00, &m68k_move<byte, index_pc_indirect, predec_indirect>},
	 {0x113c, 0xe00, &m68k_move<byte, immediate, predec_indirect>},
	 {0x1140, 0xe07, &m68k_move<byte, data_register_direct, disp_indirect>},
	 {0x1150, 0xe07, &m68k_move<byte, indirect, disp_indirect>},
	 {0x1158, 0xe07, &m68k_move<byte, postinc_indirect, disp_indirect>},
	 {0x1160, 0xe07, &m68k_move<byte, predec_indirect, disp_indirect>},
	 {0x1168, 0xe07, &m68k_move<byte, disp_indirect, disp_indirect>},
	 {0x1170, 0xe07, &m68k_move<byte, index_indirect, disp_indirect>},
	 {0x1178, 0xe00, &m68k_move<byte, absolute_short, disp_indirect>},
	 {0x1179, 0xe00, &m68k_move<byte, absolute_long, disp_indirect>},
	 {0x117a, 0xe00, &m68k_move<byte, disp_pc_indirect, disp_indirect>},
	 {0x117b, 0xe00, &m68k_move<byte, index_pc_indirect, disp_indirect>},
	 {0x117c, 0xe00, &m68k_move<byte, immediate, disp_indirect>},
	 {0x1180, 0xe07, &m68k_move<byte, data_register_direct, index_indirect>},
	 {0x1190, 0xe07, &m68k_move<byte, indirect, index_indirect>},
	 {0x1198, 0xe07, &m68k_move<byte, postinc_indirect, index_indirect>},
	 {0x11a0, 0xe07, &m68k_move<byte, predec_indirect, index_indirect>},
	 {0x11a8, 0xe07, &m68k_move<byte, disp_indirect, index_indirect>},
	 {0x11b0, 0xe07, &m68k_move<byte, index_indirect, index_indirect>},
	 {0x11b8, 0xe00, &m68k_move<byte, absolute_short, index_indirect>},
	 {0x11b9, 0xe00, &m68k_move<byte, absolute_long, index_indirect>},
	 {0x11ba, 0xe00, &m68k_move<byte, disp_pc_indirect, index_indirect>},
	 {0x11bb, 0xe00, &m68k_move<byte, index_pc_indirect, index_indirect>},
	 {0x11bc, 0xe00, &m68k_move<byte, immediate, index_indirect>},
	 {0x11c0,     7, &m68k_move<byte, data_register_direct, absolute_short>},
	 {0x11d0,     7, &m68k_move<byte, indirect, absolute_short>},
	 {0x11d8,     7, &m68k_move<byte, postinc_indirect, absolute_short>},
	 {0x11e0,     7, &m68k_move<byte, predec_indirect, absolute_short>},
	 {0x11e8,     7, &m68k_move<byte, disp_indirect, absolute_short>},
	 {0x11f0,     7, &m68k_move<byte, index_indirect, absolute_short>},
	 {0x11f8,     0, &m68k_move<byte, absolute_short, absolute_short>},
	 {0x11f9,     0, &m68k_move<byte, absolute_long, absolute_short>},
	 {0x11fa,     0, &m68k_move<byte, disp_pc_indirect, absolute_short>},
	 {0x11fb,     0, &m68k_move<byte, index_pc_indirect, absolute_short>},
	 {0x11fc,     0, &m68k_move<byte, immediate, absolute_short>},
	 {0x13c0,     7, &m68k_move<byte, data_register_direct, absolute_long>},
	 {0x13d0,     7, &m68k_move<byte, indirect, absolute_long>},
	 {0x13d8,     7, &m68k_move<byte, postinc_indirect, absolute_long>},
	 {0x13e0,     7, &m68k_move<byte, predec_indirect, absolute_long>},
	 {0x13e8,     7, &m68k_move<byte, disp_indirect, absolute_long>},
	 {0x13f0,     7, &m68k_move<byte, index_indirect, absolute_long>},
	 {0x13f8,     0, &m68k_move<byte, absolute_short, absolute_long>},
	 {0x13f9,     0, &m68k_move<byte, absolute_long, absolute_long>},
	 {0x13fa,     0, &m68k_move<byte, disp_pc_indirect, absolute_long>},
	 {0x13fb,     0, &m68k_move<byte, index_pc_indirect, absolute_long>},
	 {0x13fc,     0, &m68k_move<byte, immediate, absolute_long>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }

  void
  install_instructions_2(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x2000, 0xe07, &m68k_move<long_word, data_register_direct, data_register_direct>},
	 {0x2008, 0xe07, &m68k_move<long_word, address_register_direct, data_register_direct>},
	 {0x2010, 0xe07, &m68k_move<long_word, indirect, data_register_direct>},
	 {0x2018, 0xe07, &m68k_move<long_word, postinc_indirect, data_register_direct>},
	 {0x2020, 0xe07, &m68k_move<long_word, predec_indirect, data_register_direct>},
	 {0x2028, 0xe07, &m68k_move<long_word, disp_indirect, data_register_direct>},
	 {0x2030, 0xe07, &m68k_move<long_word, index_indirect, data_register_direct>},
	 {0x2038, 0xe00, &m68k_move<long_word, absolute_short, data_register_direct>},
	 {0x2039, 0xe00, &m68k_move<long_word, absolute_long, data_register_direct>},
	 {0x203a, 0xe00, &m68k_move<long_word, disp_pc_indirect, data_register_direct>},
	 {0x203b, 0xe00, &m68k_move<long_word, index_pc_indirect, data_register_direct>},
	 {0x203c, 0xe00, &m68k_move<long_word, immediate, data_register_direct>},
	 {0x2040, 0xe07, &m68k_movea<long_word, data_register_direct>},
	 {0x2048, 0xe07, &m68k_movea<long_word, address_register_direct>},
	 {0x2050, 0xe07, &m68k_movea<long_word, indirect>},
	 {0x2058, 0xe07, &m68k_movea<long_word, postinc_indirect>},
	 {0x2060, 0xe07, &m68k_movea<long_word, predec_indirect>},
	 {0x2068, 0xe07, &m68k_movea<long_word, disp_indirect>},
	 {0x2070, 0xe07, &m68k_movea<long_word, index_indirect>},
	 {0x2078, 0xe00, &m68k_movea<long_word, absolute_short>},
	 {0x2079, 0xe00, &m68k_movea<long_word, absolute_long>},
	 {0x207a, 0xe00, &m68k_movea<long_word, disp_pc_indirect>},
	 {0x207b, 0xe00, &m68k_movea<long_word, index_pc_indirect>},
	 {0x207c, 0xe00, &m68k_movea<long_word, immediate>},
	 {0x2080, 0xe07, &m68k_move<long_word, data_register_direct, indirect>},
	 {0x2088, 0xe07, &m68k_move<long_word, address_register_direct, indirect>},
	 {0x2090, 0xe07, &m68k_move<long_word, indirect, indirect>},
	 {0x2098, 0xe07, &m68k_move<long_word, postinc_indirect, indirect>},
	 {0x20a0, 0xe07, &m68k_move<long_word, predec_indirect, indirect>},
	 {0x20a8, 0xe07, &m68k_move<long_word, disp_indirect, indirect>},
	 {0x20b0, 0xe07, &m68k_move<long_word, index_indirect, indirect>},
	 {0x20b8, 0xe00, &m68k_move<long_word, absolute_short, indirect>},
	 {0x20b9, 0xe00, &m68k_move<long_word, absolute_long, indirect>},
	 {0x20ba, 0xe00, &m68k_move<long_word, disp_pc_indirect, indirect>},
	 {0x20bb, 0xe00, &m68k_move<long_word, index_pc_indirect, indirect>},
	 {0x20bc, 0xe00, &m68k_move<long_word, immediate, indirect>},
	 {0x20c0, 0xe07, &m68k_move<long_word, data_register_direct, postinc_indirect>},
	 {0x20c8, 0xe07, &m68k_move<long_word, address_register_direct, postinc_indirect>},
	 {0x20d0, 0xe07, &m68k_move<long_word, indirect, postinc_indirect>},
	 {0x20d8, 0xe07, &m68k_move<long_word, postinc_indirect, postinc_indirect>},
	 {0x20e0, 0xe07, &m68k_move<long_word, predec_indirect, postinc_indirect>},
	 {0x20e8, 0xe07, &m68k_move<long_word, disp_indirect, postinc_indirect>},
	 {0x20f0, 0xe07, &m68k_move<long_word, index_indirect, postinc_indirect>},
	 {0x20f8, 0xe00, &m68k_move<long_word, absolute_short, postinc_indirect>},
	 {0x20f9, 0xe00, &m68k_move<long_word, absolute_long, postinc_indirect>},
	 {0x20fa, 0xe00, &m68k_move<long_word, disp_pc_indirect, postinc_indirect>},
	 {0x20fb, 0xe00, &m68k_move<long_word, index_pc_indirect, postinc_indirect>},
	 {0x20fc, 0xe00, &m68k_move<long_word, immediate, postinc_indirect>},
	 {0x2100, 0xe07, &m68k_move<long_word, data_register_direct, predec_indirect>},
	 {0x2108, 0xe07, &m68k_move<long_word, address_register_direct, predec_indirect>},
	 {0x2110, 0xe07, &m68k_move<long_word, indirect, predec_indirect>},
	 {0x2118, 0xe07, &m68k_move<long_word, postinc_indirect, predec_indirect>},
	 {0x2120, 0xe07, &m68k_move<long_word, predec_indirect, predec_indirect>},
	 {0x2128, 0xe07, &m68k_move<long_word, disp_indirect, predec_indirect>},
	 {0x2130, 0xe07, &m68k_move<long_word, index_indirect, predec_indirect>},
	 {0x2138, 0xe00, &m68k_move<long_word, absolute_short, predec_indirect>},
	 {0x2139, 0xe00, &m68k_move<long_word, absolute_long, predec_indirect>},
	 {0x213a, 0xe00, &m68k_move<long_word, disp_pc_indirect, predec_indirect>},
	 {0x213b, 0xe00, &m68k_move<long_word, index_pc_indirect, predec_indirect>},
	 {0x213c, 0xe00, &m68k_move<long_word, immediate, predec_indirect>},
	 {0x2140, 0xe07, &m68k_move<long_word, data_register_direct, disp_indirect>},
	 {0x2148, 0xe07, &m68k_move<long_word, address_register_direct, disp_indirect>},
	 {0x2150, 0xe07, &m68k_move<long_word, indirect, disp_indirect>},
	 {0x2158, 0xe07, &m68k_move<long_word, postinc_indirect, disp_indirect>},
	 {0x2160, 0xe07, &m68k_move<long_word, predec_indirect, disp_indirect>},
	 {0x2168, 0xe07, &m68k_move<long_word, disp_indirect, disp_indirect>},
	 {0x2170, 0xe07, &m68k_move<long_word, index_indirect, disp_indirect>},
	 {0x2178, 0xe00, &m68k_move<long_word, absolute_short, disp_indirect>},
	 {0x2179, 0xe00, &m68k_move<long_word, absolute_long, disp_indirect>},
	 {0x217a, 0xe00, &m68k_move<long_word, disp_pc_indirect, disp_indirect>},
	 {0x217b, 0xe00, &m68k_move<long_word, index_pc_indirect, disp_indirect>},
	 {0x217c, 0xe00, &m68k_move<long_word, immediate, disp_indirect>},
	 {0x2180, 0xe07, &m68k_move<long_word, data_register_direct, index_indirect>},
	 {0x2188, 0xe07, &m68k_move<long_word, address_register_direct, index_indirect>},
	 {0x2190, 0xe07, &m68k_move<long_word, indirect, index_indirect>},
	 {0x2198, 0xe07, &m68k_move<long_word, postinc_indirect, index_indirect>},
	 {0x21a0, 0xe07, &m68k_move<long_word, predec_indirect, index_indirect>},
	 {0x21a8, 0xe07, &m68k_move<long_word, disp_indirect, index_indirect>},
	 {0x21b0, 0xe07, &m68k_move<long_word, index_indirect, index_indirect>},
	 {0x21b8, 0xe00, &m68k_move<long_word, absolute_short, index_indirect>},
	 {0x21b9, 0xe00, &m68k_move<long_word, absolute_long, index_indirect>},
	 {0x21ba, 0xe00, &m68k_move<long_word, disp_pc_indirect, index_indirect>},
	 {0x21bb, 0xe00, &m68k_move<long_word, index_pc_indirect, index_indirect>},
	 {0x21bc, 0xe00, &m68k_move<long_word, immediate, index_indirect>},
	 {0x21c0,     7, &m68k_move<long_word, data_register_direct, absolute_short>},
	 {0x21c8,     7, &m68k_move<long_word, address_register_direct, absolute_short>},
	 {0x21d0,     7, &m68k_move<long_word, indirect, absolute_short>},
	 {0x21d8,     7, &m68k_move<long_word, postinc_indirect, absolute_short>},
	 {0x21e0,     7, &m68k_move<long_word, predec_indirect, absolute_short>},
	 {0x21e8,     7, &m68k_move<long_word, disp_indirect, absolute_short>},
	 {0x21f0,     7, &m68k_move<long_word, index_indirect, absolute_short>},
	 {0x21f8,     0, &m68k_move<long_word, absolute_short, absolute_short>},
	 {0x21f9,     0, &m68k_move<long_word, absolute_long, absolute_short>},
	 {0x21fa,     0, &m68k_move<long_word, disp_pc_indirect, absolute_short>},
	 {0x21fb,     0, &m68k_move<long_word, index_pc_indirect, absolute_short>},
	 {0x21fc,     0, &m68k_move<long_word, immediate, absolute_short>},
	 {0x23c0,     7, &m68k_move<long_word, data_register_direct, absolute_long>},
	 {0x23c8,     7, &m68k_move<long_word, address_register_direct, absolute_long>},
	 {0x23d0,     7, &m68k_move<long_word, indirect, absolute_long>},
	 {0x23d8,     7, &m68k_move<long_word, postinc_indirect, absolute_long>},
	 {0x23e0,     7, &m68k_move<long_word, predec_indirect, absolute_long>},
	 {0x23e8,     7, &m68k_move<long_word, disp_indirect, absolute_long>},
	 {0x23f0,     7, &m68k_move<long_word, index_indirect, absolute_long>},
	 {0x23f8,     0, &m68k_move<long_word, absolute_short, absolute_long>},
	 {0x23f9,     0, &m68k_move<long_word, absolute_long, absolute_long>},
	 {0x23fa,     0, &m68k_move<long_word, disp_pc_indirect, absolute_long>},
	 {0x23fb,     0, &m68k_move<long_word, index_pc_indirect, absolute_long>},
	 {0x23fc,     0, &m68k_move<long_word, immediate, absolute_long>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }

  void
  install_instructions_3(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x3000, 0xe07, &m68k_move<word, data_register_direct, data_register_direct>},
	 {0x3008, 0xe07, &m68k_move<word, address_register_direct, data_register_direct>},
	 {0x3010, 0xe07, &m68k_move<word, indirect, data_register_direct>},
	 {0x3018, 0xe07, &m68k_move<word, postinc_indirect, data_register_direct>},
	 {0x3020, 0xe07, &m68k_move<word, predec_indirect, data_register_direct>},
	 {0x3028, 0xe07, &m68k_move<word, disp_indirect, data_register_direct>},
	 {0x3030, 0xe07, &m68k_move<word, index_indirect, data_register_direct>},
	 {0x3038, 0xe00, &m68k_move<word, absolute_short, data_register_direct>},
	 {0x3039, 0xe00, &m68k_move<word, absolute_long, data_register_direct>},
	 {0x303a, 0xe00, &m68k_move<word, disp_pc_indirect, data_register_direct>},
	 {0x303b, 0xe00, &m68k_move<word, index_pc_indirect, data_register_direct>},
	 {0x303c, 0xe00, &m68k_move<word, immediate, data_register_direct>},
	 {0x3040, 0xe07, &m68k_movea<word, data_register_direct>},
	 {0x3048, 0xe07, &m68k_movea<word, address_register_direct>},
	 {0x3050, 0xe07, &m68k_movea<word, indirect>},
	 {0x3058, 0xe07, &m68k_movea<word, postinc_indirect>},
	 {0x3060, 0xe07, &m68k_movea<word, predec_indirect>},
	 {0x3068, 0xe07, &m68k_movea<word, disp_indirect>},
	 {0x3070, 0xe07, &m68k_movea<word, index_indirect>},
	 {0x3078, 0xe00, &m68k_movea<word, absolute_short>},
	 {0x3079, 0xe00, &m68k_movea<word, absolute_long>},
	 {0x307a, 0xe00, &m68k_movea<word, disp_pc_indirect>},
	 {0x307b, 0xe00, &m68k_movea<word, index_pc_indirect>},
	 {0x307c, 0xe00, &m68k_movea<word, immediate>},
	 {0x3080, 0xe07, &m68k_move<word, data_register_direct, indirect>},
	 {0x3088, 0xe07, &m68k_move<word, address_register_direct, indirect>},
	 {0x3090, 0xe07, &m68k_move<word, indirect, indirect>},
	 {0x3098, 0xe07, &m68k_move<word, postinc_indirect, indirect>},
	 {0x30a0, 0xe07, &m68k_move<word, predec_indirect, indirect>},
	 {0x30a8, 0xe07, &m68k_move<word, disp_indirect, indirect>},
	 {0x30b0, 0xe07, &m68k_move<word, index_indirect, indirect>},
	 {0x30b8, 0xe00, &m68k_move<word, absolute_short, indirect>},
	 {0x30b9, 0xe00, &m68k_move<word, absolute_long, indirect>},
	 {0x30ba, 0xe00, &m68k_move<word, disp_pc_indirect, indirect>},
	 {0x30bb, 0xe00, &m68k_move<word, index_pc_indirect, indirect>},
	 {0x30bc, 0xe00, &m68k_move<word, immediate, indirect>},
	 {0x30c0, 0xe07, &m68k_move<word, data_register_direct, postinc_indirect>},
	 {0x30c8, 0xe07, &m68k_move<word, address_register_direct, postinc_indirect>},
	 {0x30d0, 0xe07, &m68k_move<word, indirect, postinc_indirect>},
	 {0x30d8, 0xe07, &m68k_move<word, postinc_indirect, postinc_indirect>},
	 {0x30e0, 0xe07, &m68k_move<word, predec_indirect, postinc_indirect>},
	 {0x30e8, 0xe07, &m68k_move<word, disp_indirect, postinc_indirect>},
	 {0x30f0, 0xe07, &m68k_move<word, index_indirect, postinc_indirect>},
	 {0x30f8, 0xe00, &m68k_move<word, absolute_short, postinc_indirect>},
	 {0x30f9, 0xe00, &m68k_move<word, absolute_long, postinc_indirect>},
	 {0x30fa, 0xe00, &m68k_move<word, disp_pc_indirect, postinc_indirect>},
	 {0x30fb, 0xe00, &m68k_move<word, index_pc_indirect, postinc_indirect>},
	 {0x30fc, 0xe00, &m68k_move<word, immediate, postinc_indirect>},
	 {0x3100, 0xe07, &m68k_move<word, data_register_direct, predec_indirect>},
	 {0x3108, 0xe07, &m68k_move<word, address_register_direct, predec_indirect>},
	 {0x3110, 0xe07, &m68k_move<word, indirect, predec_indirect>},
	 {0x3118, 0xe07, &m68k_move<word, postinc_indirect, predec_indirect>},
	 {0x3120, 0xe07, &m68k_move<word, predec_indirect, predec_indirect>},
	 {0x3128, 0xe07, &m68k_move<word, disp_indirect, predec_indirect>},
	 {0x3130, 0xe07, &m68k_move<word, index_indirect, predec_indirect>},
	 {0x3138, 0xe00, &m68k_move<word, absolute_short, predec_indirect>},
	 {0x3139, 0xe00, &m68k_move<word, absolute_long, predec_indirect>},
	 {0x313a, 0xe00, &m68k_move<word, disp_pc_indirect, predec_indirect>},
	 {0x313b, 0xe00, &m68k_move<word, index_pc_indirect, predec_indirect>},
	 {0x313c, 0xe00, &m68k_move<word, immediate, predec_indirect>},
	 {0x3140, 0xe07, &m68k_move<word, data_register_direct, disp_indirect>},
	 {0x3148, 0xe07, &m68k_move<word, address_register_direct, disp_indirect>},
	 {0x3150, 0xe07, &m68k_move<word, indirect, disp_indirect>},
	 {0x3158, 0xe07, &m68k_move<word, postinc_indirect, disp_indirect>},
	 {0x3160, 0xe07, &m68k_move<word, predec_indirect, disp_indirect>},
	 {0x3168, 0xe07, &m68k_move<word, disp_indirect, disp_indirect>},
	 {0x3170, 0xe07, &m68k_move<word, index_indirect, disp_indirect>},
	 {0x3178, 0xe00, &m68k_move<word, absolute_short, disp_indirect>},
	 {0x3179, 0xe00, &m68k_move<word, absolute_long, disp_indirect>},
	 {0x317a, 0xe00, &m68k_move<word, disp_pc_indirect, disp_indirect>},
	 {0x317b, 0xe00, &m68k_move<word, index_pc_indirect, disp_indirect>},
	 {0x317c, 0xe00, &m68k_move<word, immediate, disp_indirect>},
	 {0x3180, 0xe07, &m68k_move<word, data_register_direct, index_indirect>},
	 {0x3188, 0xe07, &m68k_move<word, address_register_direct, index_indirect>},
	 {0x3190, 0xe07, &m68k_move<word, indirect, index_indirect>},
	 {0x3198, 0xe07, &m68k_move<word, postinc_indirect, index_indirect>},
	 {0x31a0, 0xe07, &m68k_move<word, predec_indirect, index_indirect>},
	 {0x31a8, 0xe07, &m68k_move<word, disp_indirect, index_indirect>},
	 {0x31b0, 0xe07, &m68k_move<word, index_indirect, index_indirect>},
	 {0x31b8, 0xe00, &m68k_move<word, absolute_short, index_indirect>},
	 {0x31b9, 0xe00, &m68k_move<word, absolute_long, index_indirect>},
	 {0x31ba, 0xe00, &m68k_move<word, disp_pc_indirect, index_indirect>},
	 {0x31bb, 0xe00, &m68k_move<word, index_pc_indirect, index_indirect>},
	 {0x31bc, 0xe00, &m68k_move<word, immediate, index_indirect>},
	 {0x31c0,     7, &m68k_move<word, data_register_direct, absolute_short>},
	 {0x31c8,     7, &m68k_move<word, address_register_direct, absolute_short>},
	 {0x31d0,     7, &m68k_move<word, indirect, absolute_short>},
	 {0x31d8,     7, &m68k_move<word, postinc_indirect, absolute_short>},
	 {0x31e0,     7, &m68k_move<word, predec_indirect, absolute_short>},
	 {0x31e8,     7, &m68k_move<word, disp_indirect, absolute_short>},
	 {0x31f0,     7, &m68k_move<word, index_indirect, absolute_short>},
	 {0x31f8,     0, &m68k_move<word, absolute_short, absolute_short>},
	 {0x31f9,     0, &m68k_move<word, absolute_long, absolute_short>},
	 {0x31fa,     0, &m68k_move<word, disp_pc_indirect, absolute_short>},
	 {0x31fb,     0, &m68k_move<word, index_pc_indirect, absolute_short>},
	 {0x31fc,     0, &m68k_move<word, immediate, absolute_short>},
	 {0x33c0,     7, &m68k_move<word, data_register_direct, absolute_long>},
	 {0x33c8,     7, &m68k_move<word, address_register_direct, absolute_long>},
	 {0x33d0,     7, &m68k_move<word, indirect, absolute_long>},
	 {0x33d8,     7, &m68k_move<word, postinc_indirect, absolute_long>},
	 {0x33e0,     7, &m68k_move<word, predec_indirect, absolute_long>},
	 {0x33e8,     7, &m68k_move<word, disp_indirect, absolute_long>},
	 {0x33f0,     7, &m68k_move<word, index_indirect, absolute_long>},
	 {0x33f8,     0, &m68k_move<word, absolute_short, absolute_long>},
	 {0x33f9,     0, &m68k_move<word, absolute_long, absolute_long>},
	 {0x33fa,     0, &m68k_move<word, disp_pc_indirect, absolute_long>},
	 {0x33fb,     0, &m68k_move<word, index_pc_indirect, absolute_long>},
	 {0x33fc,     0, &m68k_move<word, immediate, absolute_long>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}