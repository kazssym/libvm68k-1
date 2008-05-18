/* inst2 - instruction group 2 for Virtual M68000 Toolkit
   Copyright (C) 1998-2008 Hypercore Software Design, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if _WIN32
#include <windows.h>
#endif

#if _WIN32
#define VM68K_PUBLIC __declspec (dllexport)
#elif __GNUC__
#define VM68K_PUBLIC __attribute__ ((__visibility__ ("default")))
#else
#define VM68K_PUBLIC
#endif

#include <vm68k/processor>

#include "transfer.h"

#include <cassert>

using namespace vx68k;
using namespace vx68k_m68k;

namespace
{
  typedef vm68k_long_word_size L;

  static const vm68k_instruction_decoder::spec inst2[] =
    {
      { 0x2000, 0xe07, move_instruction<L, d_reg_direct_addressing, d_reg_direct_addressing>::execute },
      { 0x2008, 0xe07, move_instruction<L, a_reg_direct_addressing, d_reg_direct_addressing>::execute },
      { 0x2010, 0xe07, move_instruction<L, indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x2018, 0xe07, move_instruction<L, postinc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x2020, 0xe07, move_instruction<L, predec_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x2028, 0xe07, move_instruction<L, disp_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x2030, 0xe07, move_instruction<L, index_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x2038, 0xe00, move_instruction<L, abs_short_addressing, d_reg_direct_addressing>::execute },
      { 0x2039, 0xe00, move_instruction<L, abs_long_addressing, d_reg_direct_addressing>::execute },
      { 0x203a, 0xe00, move_instruction<L, disp_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x203b, 0xe00, move_instruction<L, index_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x203c, 0xe00, move_instruction<L, immediate_addressing, d_reg_direct_addressing>::execute },
      { 0x2040, 0xe07, movea_instruction<L, d_reg_direct_addressing>::execute },
      { 0x2048, 0xe07, movea_instruction<L, a_reg_direct_addressing>::execute },
      { 0x2050, 0xe07, movea_instruction<L, indirect_addressing>::execute },
      { 0x2058, 0xe07, movea_instruction<L, postinc_indirect_addressing>::execute },
      { 0x2060, 0xe07, movea_instruction<L, predec_indirect_addressing>::execute },
      { 0x2068, 0xe07, movea_instruction<L, disp_indirect_addressing>::execute },
      { 0x2070, 0xe07, movea_instruction<L, index_indirect_addressing>::execute },
      { 0x2078, 0xe00, movea_instruction<L, abs_short_addressing>::execute },
      { 0x2079, 0xe00, movea_instruction<L, abs_long_addressing>::execute },
      { 0x207a, 0xe00, movea_instruction<L, disp_pc_indirect_addressing>::execute },
      { 0x207b, 0xe00, movea_instruction<L, index_pc_indirect_addressing>::execute },
      { 0x207c, 0xe00, movea_instruction<L, immediate_addressing>::execute },
      { 0x2080, 0xe07, move_instruction<L, d_reg_direct_addressing, indirect_addressing>::execute },
      { 0x2088, 0xe07, move_instruction<L, a_reg_direct_addressing, indirect_addressing>::execute },
      { 0x2090, 0xe07, move_instruction<L, indirect_addressing, indirect_addressing>::execute },
      { 0x2098, 0xe07, move_instruction<L, postinc_indirect_addressing, indirect_addressing>::execute },
      { 0x20a0, 0xe07, move_instruction<L, predec_indirect_addressing, indirect_addressing>::execute },
      { 0x20a8, 0xe07, move_instruction<L, disp_indirect_addressing, indirect_addressing>::execute },
      { 0x20b0, 0xe07, move_instruction<L, index_indirect_addressing, indirect_addressing>::execute },
      { 0x20b8, 0xe00, move_instruction<L, abs_short_addressing, indirect_addressing>::execute },
      { 0x20b9, 0xe00, move_instruction<L, abs_long_addressing, indirect_addressing>::execute },
      { 0x20ba, 0xe00, move_instruction<L, disp_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x20bb, 0xe00, move_instruction<L, index_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x20bc, 0xe00, move_instruction<L, immediate_addressing, indirect_addressing>::execute },
      { 0x20c0, 0xe07, move_instruction<L, d_reg_direct_addressing, postinc_indirect_addressing>::execute },
      { 0x20c8, 0xe07, move_instruction<L, a_reg_direct_addressing, postinc_indirect_addressing>::execute },
      { 0x20d0, 0xe07, move_instruction<L, indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20d8, 0xe07, move_instruction<L, postinc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20e0, 0xe07, move_instruction<L, predec_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20e8, 0xe07, move_instruction<L, disp_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20f0, 0xe07, move_instruction<L, index_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20f8, 0xe00, move_instruction<L, abs_short_addressing, postinc_indirect_addressing>::execute },
      { 0x20f9, 0xe00, move_instruction<L, abs_long_addressing, postinc_indirect_addressing>::execute },
      { 0x20fa, 0xe00, move_instruction<L, disp_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20fb, 0xe00, move_instruction<L, index_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x20fc, 0xe00, move_instruction<L, immediate_addressing, postinc_indirect_addressing>::execute },
      { 0x2100, 0xe07, move_instruction<L, d_reg_direct_addressing, predec_indirect_addressing>::execute },
      { 0x2108, 0xe07, move_instruction<L, a_reg_direct_addressing, predec_indirect_addressing>::execute },
      { 0x2110, 0xe07, move_instruction<L, indirect_addressing, predec_indirect_addressing>::execute },
      { 0x2118, 0xe07, move_instruction<L, postinc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x2120, 0xe07, move_instruction<L, predec_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x2128, 0xe07, move_instruction<L, disp_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x2130, 0xe07, move_instruction<L, index_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x2138, 0xe00, move_instruction<L, abs_short_addressing, predec_indirect_addressing>::execute },
      { 0x2139, 0xe00, move_instruction<L, abs_long_addressing, predec_indirect_addressing>::execute },
      { 0x213a, 0xe00, move_instruction<L, disp_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x213b, 0xe00, move_instruction<L, index_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x213c, 0xe00, move_instruction<L, immediate_addressing, predec_indirect_addressing>::execute },
      { 0x2140, 0xe07, move_instruction<L, d_reg_direct_addressing, disp_indirect_addressing>::execute },
      { 0x2148, 0xe07, move_instruction<L, a_reg_direct_addressing, disp_indirect_addressing>::execute },
      { 0x2150, 0xe07, move_instruction<L, indirect_addressing, disp_indirect_addressing>::execute },
      { 0x2158, 0xe07, move_instruction<L, postinc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x2160, 0xe07, move_instruction<L, predec_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x2168, 0xe07, move_instruction<L, disp_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x2170, 0xe07, move_instruction<L, index_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x2178, 0xe00, move_instruction<L, abs_short_addressing, disp_indirect_addressing>::execute },
      { 0x2179, 0xe00, move_instruction<L, abs_long_addressing, disp_indirect_addressing>::execute },
      { 0x217a, 0xe00, move_instruction<L, disp_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x217b, 0xe00, move_instruction<L, index_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x217c, 0xe00, move_instruction<L, immediate_addressing, disp_indirect_addressing>::execute },
      { 0x2180, 0xe07, move_instruction<L, d_reg_direct_addressing, index_indirect_addressing>::execute },
      { 0x2188, 0xe07, move_instruction<L, a_reg_direct_addressing, index_indirect_addressing>::execute },
      { 0x2190, 0xe07, move_instruction<L, indirect_addressing, index_indirect_addressing>::execute },
      { 0x2198, 0xe07, move_instruction<L, postinc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21a0, 0xe07, move_instruction<L, predec_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21a8, 0xe07, move_instruction<L, disp_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21b0, 0xe07, move_instruction<L, index_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21b8, 0xe00, move_instruction<L, abs_short_addressing, index_indirect_addressing>::execute },
      { 0x21b9, 0xe00, move_instruction<L, abs_long_addressing, index_indirect_addressing>::execute },
      { 0x21ba, 0xe00, move_instruction<L, disp_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21bb, 0xe00, move_instruction<L, index_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x21bc, 0xe00, move_instruction<L, immediate_addressing, index_indirect_addressing>::execute },
      { 0x21c0,     7, move_instruction<L, d_reg_direct_addressing, abs_short_addressing>::execute },
      { 0x21c8,     7, move_instruction<L, a_reg_direct_addressing, abs_short_addressing>::execute },
      { 0x21d0,     7, move_instruction<L, indirect_addressing, abs_short_addressing>::execute },
      { 0x21d8,     7, move_instruction<L, postinc_indirect_addressing, abs_short_addressing>::execute },
      { 0x21e0,     7, move_instruction<L, predec_indirect_addressing, abs_short_addressing>::execute },
      { 0x21e8,     7, move_instruction<L, disp_indirect_addressing, abs_short_addressing>::execute },
      { 0x21f0,     7, move_instruction<L, index_indirect_addressing, abs_short_addressing>::execute },
      { 0x21f8,     0, move_instruction<L, abs_short_addressing, abs_short_addressing>::execute },
      { 0x21f9,     0, move_instruction<L, abs_long_addressing, abs_short_addressing>::execute },
      { 0x21fa,     0, move_instruction<L, disp_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x21fb,     0, move_instruction<L, index_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x21fc,     0, move_instruction<L, immediate_addressing, abs_short_addressing>::execute },
      { 0x23c0,     7, move_instruction<L, d_reg_direct_addressing, abs_long_addressing>::execute },
      { 0x23c8,     7, move_instruction<L, a_reg_direct_addressing, abs_long_addressing>::execute },
      { 0x23d0,     7, move_instruction<L, indirect_addressing, abs_long_addressing>::execute },
      { 0x23d8,     7, move_instruction<L, postinc_indirect_addressing, abs_long_addressing>::execute },
      { 0x23e0,     7, move_instruction<L, predec_indirect_addressing, abs_long_addressing>::execute },
      { 0x23e8,     7, move_instruction<L, disp_indirect_addressing, abs_long_addressing>::execute },
      { 0x23f0,     7, move_instruction<L, index_indirect_addressing, abs_long_addressing>::execute },
      { 0x23f8,     0, move_instruction<L, abs_short_addressing, abs_long_addressing>::execute },
      { 0x23f9,     0, move_instruction<L, abs_long_addressing, abs_long_addressing>::execute },
      { 0x23fa,     0, move_instruction<L, disp_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x23fb,     0, move_instruction<L, index_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x23fc,     0, move_instruction<L, immediate_addressing, abs_long_addressing>::execute },
    };
}

namespace vx68k
{
  void vm68k_instruction_decoder::insert_inst2 (vm68k_instruction_decoder *p)
  {
    assert (p != NULL);
    p->insert (inst2 + 0, inst2 + sizeof inst2 / sizeof inst2[0]);
  }
}
