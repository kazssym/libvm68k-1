/* inst3 - instruction group 3 for Virtual M68000 Toolkit
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
  typedef vm68k_word_size W;

  static const vm68k_instruction_decoder::spec inst3[] =
    {
      { 0x3000, 0xe07, move_instruction<W, d_reg_direct_addressing, d_reg_direct_addressing>::execute },
      { 0x3008, 0xe07, move_instruction<W, a_reg_direct_addressing, d_reg_direct_addressing>::execute },
      { 0x3010, 0xe07, move_instruction<W, indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x3018, 0xe07, move_instruction<W, postinc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x3020, 0xe07, move_instruction<W, predec_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x3028, 0xe07, move_instruction<W, disp_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x3030, 0xe07, move_instruction<W, index_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x3038, 0xe00, move_instruction<W, abs_short_addressing, d_reg_direct_addressing>::execute },
      { 0x3039, 0xe00, move_instruction<W, abs_long_addressing, d_reg_direct_addressing>::execute },
      { 0x303a, 0xe00, move_instruction<W, disp_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x303b, 0xe00, move_instruction<W, index_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x303c, 0xe00, move_instruction<W, immediate_addressing, d_reg_direct_addressing>::execute },
      { 0x3040, 0xe07, movea_instruction<W, d_reg_direct_addressing>::execute },
      { 0x3048, 0xe07, movea_instruction<W, a_reg_direct_addressing>::execute },
      { 0x3050, 0xe07, movea_instruction<W, indirect_addressing>::execute },
      { 0x3058, 0xe07, movea_instruction<W, postinc_indirect_addressing>::execute },
      { 0x3060, 0xe07, movea_instruction<W, predec_indirect_addressing>::execute },
      { 0x3068, 0xe07, movea_instruction<W, disp_indirect_addressing>::execute },
      { 0x3070, 0xe07, movea_instruction<W, index_indirect_addressing>::execute },
      { 0x3078, 0xe00, movea_instruction<W, abs_short_addressing>::execute },
      { 0x3079, 0xe00, movea_instruction<W, abs_long_addressing>::execute },
      { 0x307a, 0xe00, movea_instruction<W, disp_pc_indirect_addressing>::execute },
      { 0x307b, 0xe00, movea_instruction<W, index_pc_indirect_addressing>::execute },
      { 0x307c, 0xe00, movea_instruction<W, immediate_addressing>::execute },
      { 0x3080, 0xe07, move_instruction<W, d_reg_direct_addressing, indirect_addressing>::execute },
      { 0x3088, 0xe07, move_instruction<W, a_reg_direct_addressing, indirect_addressing>::execute },
      { 0x3090, 0xe07, move_instruction<W, indirect_addressing, indirect_addressing>::execute },
      { 0x3098, 0xe07, move_instruction<W, postinc_indirect_addressing, indirect_addressing>::execute },
      { 0x30a0, 0xe07, move_instruction<W, predec_indirect_addressing, indirect_addressing>::execute },
      { 0x30a8, 0xe07, move_instruction<W, disp_indirect_addressing, indirect_addressing>::execute },
      { 0x30b0, 0xe07, move_instruction<W, index_indirect_addressing, indirect_addressing>::execute },
      { 0x30b8, 0xe00, move_instruction<W, abs_short_addressing, indirect_addressing>::execute },
      { 0x30b9, 0xe00, move_instruction<W, abs_long_addressing, indirect_addressing>::execute },
      { 0x30ba, 0xe00, move_instruction<W, disp_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x30bb, 0xe00, move_instruction<W, index_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x30bc, 0xe00, move_instruction<W, immediate_addressing, indirect_addressing>::execute },
      { 0x30c0, 0xe07, move_instruction<W, d_reg_direct_addressing, postinc_indirect_addressing>::execute },
      { 0x30c8, 0xe07, move_instruction<W, a_reg_direct_addressing, postinc_indirect_addressing>::execute },
      { 0x30d0, 0xe07, move_instruction<W, indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30d8, 0xe07, move_instruction<W, postinc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30e0, 0xe07, move_instruction<W, predec_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30e8, 0xe07, move_instruction<W, disp_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30f0, 0xe07, move_instruction<W, index_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30f8, 0xe00, move_instruction<W, abs_short_addressing, postinc_indirect_addressing>::execute },
      { 0x30f9, 0xe00, move_instruction<W, abs_long_addressing, postinc_indirect_addressing>::execute },
      { 0x30fa, 0xe00, move_instruction<W, disp_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30fb, 0xe00, move_instruction<W, index_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x30fc, 0xe00, move_instruction<W, immediate_addressing, postinc_indirect_addressing>::execute },
      { 0x3100, 0xe07, move_instruction<W, d_reg_direct_addressing, predec_indirect_addressing>::execute },
      { 0x3108, 0xe07, move_instruction<W, a_reg_direct_addressing, predec_indirect_addressing>::execute },
      { 0x3110, 0xe07, move_instruction<W, indirect_addressing, predec_indirect_addressing>::execute },
      { 0x3118, 0xe07, move_instruction<W, postinc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x3120, 0xe07, move_instruction<W, predec_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x3128, 0xe07, move_instruction<W, disp_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x3130, 0xe07, move_instruction<W, index_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x3138, 0xe00, move_instruction<W, abs_short_addressing, predec_indirect_addressing>::execute },
      { 0x3139, 0xe00, move_instruction<W, abs_long_addressing, predec_indirect_addressing>::execute },
      { 0x313a, 0xe00, move_instruction<W, disp_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x313b, 0xe00, move_instruction<W, index_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x313c, 0xe00, move_instruction<W, immediate_addressing, predec_indirect_addressing>::execute },
      { 0x3140, 0xe07, move_instruction<W, d_reg_direct_addressing, disp_indirect_addressing>::execute },
      { 0x3148, 0xe07, move_instruction<W, a_reg_direct_addressing, disp_indirect_addressing>::execute },
      { 0x3150, 0xe07, move_instruction<W, indirect_addressing, disp_indirect_addressing>::execute },
      { 0x3158, 0xe07, move_instruction<W, postinc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x3160, 0xe07, move_instruction<W, predec_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x3168, 0xe07, move_instruction<W, disp_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x3170, 0xe07, move_instruction<W, index_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x3178, 0xe00, move_instruction<W, abs_short_addressing, disp_indirect_addressing>::execute },
      { 0x3179, 0xe00, move_instruction<W, abs_long_addressing, disp_indirect_addressing>::execute },
      { 0x317a, 0xe00, move_instruction<W, disp_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x317b, 0xe00, move_instruction<W, index_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x317c, 0xe00, move_instruction<W, immediate_addressing, disp_indirect_addressing>::execute },
      { 0x3180, 0xe07, move_instruction<W, d_reg_direct_addressing, index_indirect_addressing>::execute },
      { 0x3188, 0xe07, move_instruction<W, a_reg_direct_addressing, index_indirect_addressing>::execute },
      { 0x3190, 0xe07, move_instruction<W, indirect_addressing, index_indirect_addressing>::execute },
      { 0x3198, 0xe07, move_instruction<W, postinc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31a0, 0xe07, move_instruction<W, predec_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31a8, 0xe07, move_instruction<W, disp_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31b0, 0xe07, move_instruction<W, index_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31b8, 0xe00, move_instruction<W, abs_short_addressing, index_indirect_addressing>::execute },
      { 0x31b9, 0xe00, move_instruction<W, abs_long_addressing, index_indirect_addressing>::execute },
      { 0x31ba, 0xe00, move_instruction<W, disp_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31bb, 0xe00, move_instruction<W, index_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x31bc, 0xe00, move_instruction<W, immediate_addressing, index_indirect_addressing>::execute },
      { 0x31c0,     7, move_instruction<W, d_reg_direct_addressing, abs_short_addressing>::execute },
      { 0x31c8,     7, move_instruction<W, a_reg_direct_addressing, abs_short_addressing>::execute },
      { 0x31d0,     7, move_instruction<W, indirect_addressing, abs_short_addressing>::execute },
      { 0x31d8,     7, move_instruction<W, postinc_indirect_addressing, abs_short_addressing>::execute },
      { 0x31e0,     7, move_instruction<W, predec_indirect_addressing, abs_short_addressing>::execute },
      { 0x31e8,     7, move_instruction<W, disp_indirect_addressing, abs_short_addressing>::execute },
      { 0x31f0,     7, move_instruction<W, index_indirect_addressing, abs_short_addressing>::execute },
      { 0x31f8,     0, move_instruction<W, abs_short_addressing, abs_short_addressing>::execute },
      { 0x31f9,     0, move_instruction<W, abs_long_addressing, abs_short_addressing>::execute },
      { 0x31fa,     0, move_instruction<W, disp_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x31fb,     0, move_instruction<W, index_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x31fc,     0, move_instruction<W, immediate_addressing, abs_short_addressing>::execute },
      { 0x33c0,     7, move_instruction<W, d_reg_direct_addressing, abs_long_addressing>::execute },
      { 0x33c8,     7, move_instruction<W, a_reg_direct_addressing, abs_long_addressing>::execute },
      { 0x33d0,     7, move_instruction<W, indirect_addressing, abs_long_addressing>::execute },
      { 0x33d8,     7, move_instruction<W, postinc_indirect_addressing, abs_long_addressing>::execute },
      { 0x33e0,     7, move_instruction<W, predec_indirect_addressing, abs_long_addressing>::execute },
      { 0x33e8,     7, move_instruction<W, disp_indirect_addressing, abs_long_addressing>::execute },
      { 0x33f0,     7, move_instruction<W, index_indirect_addressing, abs_long_addressing>::execute },
      { 0x33f8,     0, move_instruction<W, abs_short_addressing, abs_long_addressing>::execute },
      { 0x33f9,     0, move_instruction<W, abs_long_addressing, abs_long_addressing>::execute },
      { 0x33fa,     0, move_instruction<W, disp_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x33fb,     0, move_instruction<W, index_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x33fc,     0, move_instruction<W, immediate_addressing, abs_long_addressing>::execute },
    };
}

namespace vx68k
{
  void vm68k_instruction_decoder::insert_inst3 (vm68k_instruction_decoder *p)
  {
    assert (p != NULL);
    p->insert (inst3 + 0, inst3 + sizeof inst3 / sizeof inst3[0]);
  }
}
