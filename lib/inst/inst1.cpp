/* inst1 - instruction group 1 for Virtual M68000 Toolkit
 * Copyright (C) 1998-2008 Hypercore Software Design, Ltd.
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
  typedef vm68k_byte_size B;

  static const vm68k_instruction_decoder::spec inst1[] =
    {
      { 0x1000, 0xe07, move_instruction<B, d_reg_direct_addressing, d_reg_direct_addressing>::execute },
      { 0x1010, 0xe07, move_instruction<B, indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x1018, 0xe07, move_instruction<B, postinc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x1020, 0xe07, move_instruction<B, predec_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x1028, 0xe07, move_instruction<B, disp_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x1030, 0xe07, move_instruction<B, index_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x1038, 0xe00, move_instruction<B, abs_short_addressing, d_reg_direct_addressing>::execute },
      { 0x1039, 0xe00, move_instruction<B, abs_long_addressing, d_reg_direct_addressing>::execute },
      { 0x103a, 0xe00, move_instruction<B, disp_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x103b, 0xe00, move_instruction<B, index_pc_indirect_addressing, d_reg_direct_addressing>::execute },
      { 0x103c, 0xe00, move_instruction<B, immediate_addressing, d_reg_direct_addressing>::execute },
      { 0x1080, 0xe07, move_instruction<B, d_reg_direct_addressing, indirect_addressing>::execute },
      { 0x1090, 0xe07, move_instruction<B, indirect_addressing, indirect_addressing>::execute },
      { 0x1098, 0xe07, move_instruction<B, postinc_indirect_addressing, indirect_addressing>::execute },
      { 0x10a0, 0xe07, move_instruction<B, predec_indirect_addressing, indirect_addressing>::execute },
      { 0x10a8, 0xe07, move_instruction<B, disp_indirect_addressing, indirect_addressing>::execute },
      { 0x10b0, 0xe07, move_instruction<B, index_indirect_addressing, indirect_addressing>::execute },
      { 0x10b8, 0xe00, move_instruction<B, abs_short_addressing, indirect_addressing>::execute },
      { 0x10b9, 0xe00, move_instruction<B, abs_long_addressing, indirect_addressing>::execute },
      { 0x10ba, 0xe00, move_instruction<B, disp_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x10bb, 0xe00, move_instruction<B, index_pc_indirect_addressing, indirect_addressing>::execute },
      { 0x10bc, 0xe00, move_instruction<B, immediate_addressing, indirect_addressing>::execute },
      { 0x10c0, 0xe07, move_instruction<B, d_reg_direct_addressing, postinc_indirect_addressing>::execute },
      { 0x10d0, 0xe07, move_instruction<B, indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10d8, 0xe07, move_instruction<B, postinc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10e0, 0xe07, move_instruction<B, predec_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10e8, 0xe07, move_instruction<B, disp_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10f0, 0xe07, move_instruction<B, index_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10f8, 0xe00, move_instruction<B, abs_short_addressing, postinc_indirect_addressing>::execute },
      { 0x10f9, 0xe00, move_instruction<B, abs_long_addressing, postinc_indirect_addressing>::execute },
      { 0x10fa, 0xe00, move_instruction<B, disp_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10fb, 0xe00, move_instruction<B, index_pc_indirect_addressing, postinc_indirect_addressing>::execute },
      { 0x10fc, 0xe00, move_instruction<B, immediate_addressing, postinc_indirect_addressing>::execute },
      { 0x1100, 0xe07, move_instruction<B, d_reg_direct_addressing, predec_indirect_addressing>::execute },
      { 0x1110, 0xe07, move_instruction<B, indirect_addressing, predec_indirect_addressing>::execute },
      { 0x1118, 0xe07, move_instruction<B, postinc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x1120, 0xe07, move_instruction<B, predec_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x1128, 0xe07, move_instruction<B, disp_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x1130, 0xe07, move_instruction<B, index_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x1138, 0xe00, move_instruction<B, abs_short_addressing, predec_indirect_addressing>::execute },
      { 0x1139, 0xe00, move_instruction<B, abs_long_addressing, predec_indirect_addressing>::execute },
      { 0x113a, 0xe00, move_instruction<B, disp_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x113b, 0xe00, move_instruction<B, index_pc_indirect_addressing, predec_indirect_addressing>::execute },
      { 0x113c, 0xe00, move_instruction<B, immediate_addressing, predec_indirect_addressing>::execute },
      { 0x1140, 0xe07, move_instruction<B, d_reg_direct_addressing, disp_indirect_addressing>::execute },
      { 0x1150, 0xe07, move_instruction<B, indirect_addressing, disp_indirect_addressing>::execute },
      { 0x1158, 0xe07, move_instruction<B, postinc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x1160, 0xe07, move_instruction<B, predec_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x1168, 0xe07, move_instruction<B, disp_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x1170, 0xe07, move_instruction<B, index_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x1178, 0xe00, move_instruction<B, abs_short_addressing, disp_indirect_addressing>::execute },
      { 0x1179, 0xe00, move_instruction<B, abs_long_addressing, disp_indirect_addressing>::execute },
      { 0x117a, 0xe00, move_instruction<B, disp_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x117b, 0xe00, move_instruction<B, index_pc_indirect_addressing, disp_indirect_addressing>::execute },
      { 0x117c, 0xe00, move_instruction<B, immediate_addressing, disp_indirect_addressing>::execute },
      { 0x1180, 0xe07, move_instruction<B, d_reg_direct_addressing, index_indirect_addressing>::execute },
      { 0x1190, 0xe07, move_instruction<B, indirect_addressing, index_indirect_addressing>::execute },
      { 0x1198, 0xe07, move_instruction<B, postinc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11a0, 0xe07, move_instruction<B, predec_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11a8, 0xe07, move_instruction<B, disp_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11b0, 0xe07, move_instruction<B, index_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11b8, 0xe00, move_instruction<B, abs_short_addressing, index_indirect_addressing>::execute },
      { 0x11b9, 0xe00, move_instruction<B, abs_long_addressing, index_indirect_addressing>::execute },
      { 0x11ba, 0xe00, move_instruction<B, disp_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11bb, 0xe00, move_instruction<B, index_pc_indirect_addressing, index_indirect_addressing>::execute },
      { 0x11bc, 0xe00, move_instruction<B, immediate_addressing, index_indirect_addressing>::execute },
      { 0x11c0,     7, move_instruction<B, d_reg_direct_addressing, abs_short_addressing>::execute },
      { 0x11d0,     7, move_instruction<B, indirect_addressing, abs_short_addressing>::execute },
      { 0x11d8,     7, move_instruction<B, postinc_indirect_addressing, abs_short_addressing>::execute },
      { 0x11e0,     7, move_instruction<B, predec_indirect_addressing, abs_short_addressing>::execute },
      { 0x11e8,     7, move_instruction<B, disp_indirect_addressing, abs_short_addressing>::execute },
      { 0x11f0,     7, move_instruction<B, index_indirect_addressing, abs_short_addressing>::execute },
      { 0x11f8,     0, move_instruction<B, abs_short_addressing, abs_short_addressing>::execute },
      { 0x11f9,     0, move_instruction<B, abs_long_addressing, abs_short_addressing>::execute },
      { 0x11fa,     0, move_instruction<B, disp_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x11fb,     0, move_instruction<B, index_pc_indirect_addressing, abs_short_addressing>::execute },
      { 0x11fc,     0, move_instruction<B, immediate_addressing, abs_short_addressing>::execute },
      { 0x13c0,     7, move_instruction<B, d_reg_direct_addressing, abs_long_addressing>::execute },
      { 0x13d0,     7, move_instruction<B, indirect_addressing, abs_long_addressing>::execute },
      { 0x13d8,     7, move_instruction<B, postinc_indirect_addressing, abs_long_addressing>::execute },
      { 0x13e0,     7, move_instruction<B, predec_indirect_addressing, abs_long_addressing>::execute },
      { 0x13e8,     7, move_instruction<B, disp_indirect_addressing, abs_long_addressing>::execute },
      { 0x13f0,     7, move_instruction<B, index_indirect_addressing, abs_long_addressing>::execute },
      { 0x13f8,     0, move_instruction<B, abs_short_addressing, abs_long_addressing>::execute },
      { 0x13f9,     0, move_instruction<B, abs_long_addressing, abs_long_addressing>::execute },
      { 0x13fa,     0, move_instruction<B, disp_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x13fb,     0, move_instruction<B, index_pc_indirect_addressing, abs_long_addressing>::execute },
      { 0x13fc,     0, move_instruction<B, immediate_addressing, abs_long_addressing>::execute },
    };
}

namespace vx68k
{
  void vm68k_instruction_decoder::insert_inst1 (vm68k_instruction_decoder *p)
  {
    assert (p != NULL);
    p->insert (inst1 + 0, inst1 + sizeof inst1 / sizeof inst1[0]);
  }
}
