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

    /* Handles a CLR instruction.  */
    template <class Size, class Destination>
    uint32_type
    _clr(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tclr%s %s\n", Size::suffix(), ea1.text(c).c_str());
#endif

      ea1.put(c, 0);
      c.regs.ccr.set_cc(0);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }

    /* Handles an EXT instruction.  */
    template <class Size1, class Size2>
    uint32_type
    _ext(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
#ifdef L
      L("\text%s %%d%u\n", Size2::suffix(), reg1);
#endif

      typename Size2::svalue_type value = Size1::get(c.regs.d[reg1]);
      Size2::put(c.regs.d[reg1], value);
      c.regs.ccr.set_cc(value);

      return pc + 2;
    }

    /* Handles a JMP instruction.  */
    template <class Destination>
    uint32_type
    _jmp(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tjmp %s\n", ea1.text(c).c_str());
#endif

      // The condition codes are not affected by this instruction.
      uint32_type address = ea1.address(c);

      return address;
    }

    /* Handles a JSR instruction.  */
    template <class Destination>
    uint32_type
    _jsr(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tjsr %s\n", ea1.text(c).c_str());
#endif

      // XXX: The condition codes are not affected.
      uint32_type address = ea1.address(c);
      function_code fc = c.data_fc();
      uint32_type sp = c.regs.a[7] - long_word::aligned_value_size();
      long_word::put(*c.mem, fc, sp,
			  pc + 2 + Destination::extension_size());
      long_word::put(c.regs.a[7], sp);

      return address;
    }

    /* Handles a LEA instruction.  */
    template <class Destination>
    uint32_type
    _lea(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
      int reg2 = w >> 9 & 7;
#ifdef L
      L("\tlea%s %s,%%a%u\n", long_word::suffix(), ea1.text(c).c_str(),
	reg2);
#endif

      // XXX: The condition codes are not affected.
      uint32_type address = ea1.address(c);
      long_word::put(c.regs.a[reg2], address);

      return pc + 2 + Destination::extension_size();
    }

    /* Handles a LINK instruction.  */
    uint32_type
    _link(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      word::svalue_type disp = c.fetch_s(word(), pc + 2);
#ifdef L
      L("\tlink %%a%u,#%#x\n", reg1, word::uvalue(disp));
#endif

      // XXX: The condition codes are not affected.
      function_code fc = c.data_fc();
      uint32_type fp = c.regs.a[7] - long_word::aligned_value_size();
      long_word::put(*c.mem, fc, fp, c.regs.a[reg1]);
      long_word::put(c.regs.a[7], fp + disp);
      long_word::put(c.regs.a[reg1], fp);

      return pc + 2 + word::aligned_value_size();
    }

    /* Handles a MOVE-from-SR instruction.  */
    template <class Destination>
    uint32_type
    _move_from_sr(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tmove%s %%sr,%s\n", word::suffix(), ea1.text(c).c_str());
#endif

      // This instruction is not privileged on MC68000.
      // This instruction does not affect the condition codes.
      word::uvalue_type value = c.sr();
      ea1.put(c, value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a MOVE-to-SR instruction.  */
    template <class Source>
    uint32_type
    _move_to_sr(uint32_type pc, context &c, uint16_type w, void *)
    {
      Source ea1(w & 7, pc + 2);
#ifdef L
      L("\tmove%s %s,%%sr\n", word::suffix(), ea1.text(c).c_str());
#endif

      // This instruction is privileged.
      if (!c.supervisor_state())
	throw privilege_violation_exception(pc);

      // This instruction sets the condition codes.
      word::uvalue_type value = ea1.get(c);
      c.set_sr(value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a MOVE-from-USP instruction.  */
    uint32_type
    _move_from_usp(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
#ifdef L
      L("\tmove%s %%usp,%%a%u\n", long_word::suffix(), reg1);
#endif

      // This instruction is privileged.
      if (!c.supervisor_state())
	throw privilege_violation_exception(pc);

      // The condition codes are not affected by this instruction.
      c.regs.a[reg1] = c.regs.usp;

      return pc + 2;
    }

    /* Handles a MOVE-to-USP instruction.  */
    uint32_type
    _move_to_usp(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
#ifdef L
      L("\tmove%s ", long_word::suffix());
      L("%%a%u,", reg1);
      L("%%usp\n");
#endif

      // This instruction is privileged.
      if (!c.supervisor_state())
	throw privilege_violation_exception(pc);

      // This instruction does not affect the condition codes.
      long_word::put(c.regs.usp, long_word::get(c.regs.a[reg1]));

      return pc + 2;
    }

    /* Handles a MOVEM instruction (register to memory) */
    template <class Size, class Destination>
    uint32_type
    _movem_r_m(uint32_type pc, context &c, uint16_type w, void *)
    {
      word::uvalue_type mask = c.fetch_u(word(), pc + 2);
      Destination ea1(w & 7, pc + 2 + 2);
#ifdef L
      L("\tmovem%s #%#x,%s\n", Size::suffix(), mask, ea1.text(c).c_str());
#endif

      // This instruction does not affect the condition codes.
      uint16_type m = 1;
      function_code fc = c.data_fc();
      uint32_type address = ea1.address(c);
      for (uint32_type *i = c.regs.d + 0; i != c.regs.d + 8; ++i)
	{
	  if (mask & m)
	    {
	      Size::put(*c.mem, fc, address, Size::get(*i));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}
      for (uint32_type *i = c.regs.a + 0; i != c.regs.a + 8; ++i)
	{
	  if (mask & m)
	    {
	      Size::put(*c.mem, fc, address, Size::get(*i));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}

      return pc + 2 + 2 + ea1.extension_size();
    }

    /* Handles a MOVEM instruction (register to predec memory).  */
    template <class Size>
    uint32_type
    _movem_r_predec(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      word::uvalue_type mask = c.fetch_u(word(), pc + 2);
#ifdef L
      L("\tmovem%s #%#x,%%a%u@-\n", Size::suffix(), mask, reg1);
#endif

      // This instruction does not affect the condition codes.
      uint16_type m = 1;
      function_code fc = c.data_fc();
      sint32_type address = long_word::get(c.regs.a[reg1]);
      // This instruction iterates registers in reverse.
      for (uint32_type *i = c.regs.a + 8; i != c.regs.a + 0; --i)
	{
	  if (mask & m)
	    {
	      address -= Size::value_size();
	      Size::put(*c.mem, fc, address, long_word::get(*(i - 1)));
	    }
	  m <<= 1;
	}
      for (uint32_type *i = c.regs.d + 8; i != c.regs.d + 0; --i)
	{
	  if (mask & m)
	    {
	      address -= Size::value_size();
	      Size::put(*c.mem, fc, address, long_word::get(*(i - 1)));
	    }
	  m <<= 1;
	}

      long_word::put(c.regs.a[reg1], address);
      return pc + 2 + 2;
    }

    /* Handles a MOVEM instruction (memory to register).  */
    template <class Size, class Source>
    uint32_type
    _movem_m_r(uint32_type pc, context &c, uint16_type w, void *)
    {
      word::uvalue_type mask = c.fetch_u(word(), pc + 2);
      Source ea1(w & 7, pc + 2 + word::aligned_value_size());
#ifdef L
      L("\tmovem%s %s,#%#x\n", Size::suffix(), ea1.text(c).c_str(), mask);
#endif

      // XXX: The condition codes are not affected.
      uint16_type m = 1;
      function_code fc = c.data_fc();
      uint32_type address = ea1.address(c);
      for (uint32_type *i = c.regs.d + 0; i != c.regs.d + 8; ++i)
	{
	  if (mask & m)
	    {
	      long_word::put(*i, Size::get(*c.mem, fc, address));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}
      for (uint32_type *i = c.regs.a + 0; i != c.regs.a + 8; ++i)
	{
	  if (mask & m)
	    {
	      long_word::put(*i, Size::get(*c.mem, fc, address));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}

      return pc + 2 + word::aligned_value_size()
	+ Source::extension_size();
    }

    /* Handles a MOVEM instruction (postinc memory to register).  */
    template <class Size>
    uint32_type
    _movem_postinc_r(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
      word::uvalue_type mask = c.fetch_u(word(), pc + 2);
#ifdef L
      L("\tmovem%s %%a%u@+,#%#x\n", Size::suffix(), reg1, mask);
#endif

      // This instruction does not affect the condition codes.
      uint16_type m = 1;
      function_code fc = c.data_fc();
      sint32_type address = long_word::get(c.regs.a[reg1]);
      // This instruction sign-extends words to long words.
      for (uint32_type *i = c.regs.d + 0; i != c.regs.d + 8; ++i)
	{
	  if (mask & m)
	    {
	      long_word::put(*i, Size::get(*c.mem, fc, address));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}
      for (uint32_type *i = c.regs.a + 0; i != c.regs.a + 8; ++i)
	{
	  if (mask & m)
	    {
	      long_word::put(*i, Size::get(*c.mem, fc, address));
	      address += Size::value_size();
	    }
	  m <<= 1;
	}

      long_word::put(c.regs.a[reg1], address);
      return pc + 2 + 2;
    }

    /* Handles a NEG instruction.  */
    template <class Size, class Destination>
    uint32_type
    _neg(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tneg%s %s\n", Size::suffix(), ea1.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::svalue(-value1);
      ea1.put(c, value);
      c.regs.ccr.set_cc_sub(value, 0, value1);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }

    /* Handles a NOP instruction.  */
    uint32_type
    _nop(uint32_type pc, context &c, uint16_type w, void *)
    {
#ifdef L
      L("\tnop\n");
#endif

      return pc + 2;
    }

    /* Handles a NOT instruction.  */
    template <class Size, class Destination>
    uint32_type
    _not(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tnot%s %s\n", Size::suffix(), ea1.text(c).c_str());
#endif

      typename Size::svalue_type value1 = ea1.get(c);
      typename Size::svalue_type value = Size::svalue(~Size::uvalue(value1));
      ea1.put(c, value);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + ea1.extension_size();
    }

    /* Handles a PEA instruction.  */
    template <class Destination>
    uint32_type
    _pea(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\tpea%s %s\n", long_word::suffix(), ea1.text(c).c_str());
#endif

      // XXX: The condition codes are not affected.
      uint32_type address = ea1.address(c);
      function_code fc = c.data_fc();
      uint32_type sp = c.regs.a[7] - long_word::aligned_value_size();
      long_word::put(*c.mem, fc, sp, address);
      long_word::put(c.regs.a[7], sp);

      return pc + 2 + Destination::extension_size();
    }

    /* Handles a RTE instruction.  */
    uint32_type
    _rte(uint32_type pc, context &c, uint16_type w, void *)
    {
#ifdef L
      L("\trte\n");
#endif

      // This instruction is privileged.
      if (!c.supervisor_state())
	throw privilege_violation_exception(pc);

      function_code fc = c.data_fc();
      uint16_type status = word::uget(*c.mem, fc, c.regs.a[7] + 0);
      uint32_type address = long_word::uget(*c.mem, fc, c.regs.a[7] + 2);
      c.regs.a[7] += 6;
      c.set_sr(status);

      return address;
    }

    /* Handles a RTS instruction.  */
    uint32_type
    _rts(uint32_type pc, context &c, uint16_type w, void *)
    {
#ifdef L
      L("\trts\n");
#endif

      // XXX: The condition codes are not affected.
      function_code fc = c.data_fc();
      sint32_type address = long_word::get(*c.mem, fc, c.regs.a[7]);
      long_word::put(c.regs.a[7], c.regs.a[7] + 4);

      return address;
    }

    /* Handles a SWAP instruction.  */
    uint32_type
    _swap(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
#ifdef L
      L("\tswap%s %%d%u\n", word::suffix(), reg1);
#endif

      long_word::svalue_type value1 = long_word::get(c.regs.d[reg1]);
      long_word::svalue_type value
	= long_word::svalue(long_word::uvalue(value1) << 16
				 | (long_word::uvalue(value1) >> 16
				    & 0xffff));
      long_word::put(c.regs.d[reg1], value);
      c.regs.ccr.set_cc(value);

      return pc + 2;
    }

    /* Handles a TST instruction.  */
    template <class Size, class Destination>
    uint32_type
    _tst(uint32_type pc, context &c, uint16_type w, void *)
    {
      Destination ea1(w & 7, pc + 2);
#ifdef L
      L("\ttst%s %s\n", Size::suffix(), ea1.text(c).c_str());
#endif

      typename Size::svalue_type value = ea1.get(c);
      c.regs.ccr.set_cc(value);

      ea1.finish(c);
      return pc + 2 + Destination::extension_size();
    }

    /* Handles a UNLK instruction.  */
    uint32_type
    _unlk(uint32_type pc, context &c, uint16_type w, void *)
    {
      int reg1 = w & 7;
#ifdef L
      L("\tunlk %%a%u\n", reg1);
#endif

      // XXX: The condition codes are not affected.
      function_code fc = c.data_fc();
      uint32_type fp = c.regs.a[reg1];
      long_word::put(c.regs.a[reg1], long_word::get(*c.mem, fc, fp));
      long_word::put(c.regs.a[7],
			  fp + long_word::aligned_value_size());

      return pc + 2;
    }
  }
  
  using namespace instr;

  void
  install_instructions_4(processor &p, void *data)
  {
    static const instruction_map inst[]
      = {{0x40c0,     7, &_move_from_sr<word_d_register>},
	 {0x40d0,     7, &_move_from_sr<word_indirect>},
	 {0x40d8,     7, &_move_from_sr<word_postinc_indirect>},
	 {0x40e0,     7, &_move_from_sr<word_predec_indirect>},
	 {0x40e8,     7, &_move_from_sr<word_disp_indirect>},
	 {0x40f0,     7, &_move_from_sr<word_index_indirect>},
	 {0x40f8,     0, &_move_from_sr<word_abs_short>},
	 {0x40f9,     0, &_move_from_sr<word_abs_long>},
	 {0x41d0, 0xe07, &_lea<word_indirect>},
	 {0x41e8, 0xe07, &_lea<word_disp_indirect>},
	 {0x41f0, 0xe07, &_lea<word_index_indirect>},
	 {0x41f8, 0xe00, &_lea<word_abs_short>},
	 {0x41f9, 0xe00, &_lea<word_abs_long>},
	 {0x41fa, 0xe00, &_lea<word_disp_pc_indirect>},
	 {0x41fb, 0xe00, &_lea<word_index_pc_indirect>},
	 {0x4200,     7, &_clr<byte, byte_d_register>},
	 {0x4210,     7, &_clr<byte, byte_indirect>},
	 {0x4218,     7, &_clr<byte, byte_postinc_indirect>},
	 {0x4220,     7, &_clr<byte, byte_predec_indirect>},
	 {0x4228,     7, &_clr<byte, byte_disp_indirect>},
	 {0x4230,     7, &_clr<byte, byte_index_indirect>},
	 {0x4238,     0, &_clr<byte, byte_abs_short>},
	 {0x4239,     0, &_clr<byte, byte_abs_long>},
	 {0x4240,     7, &_clr<word, word_d_register>},
	 {0x4250,     7, &_clr<word, word_indirect>},
	 {0x4258,     7, &_clr<word, word_postinc_indirect>},
	 {0x4260,     7, &_clr<word, word_predec_indirect>},
	 {0x4268,     7, &_clr<word, word_disp_indirect>},
	 {0x4270,     7, &_clr<word, word_index_indirect>},
	 {0x4278,     0, &_clr<word, word_abs_short>},
	 {0x4279,     0, &_clr<word, word_abs_long>},
	 {0x4280,     7, &_clr<long_word, long_word_d_register>},
	 {0x4290,     7, &_clr<long_word, long_word_indirect>},
	 {0x4298,     7, &_clr<long_word, long_word_postinc_indirect>},
	 {0x42a0,     7, &_clr<long_word, long_word_predec_indirect>},
	 {0x42a8,     7, &_clr<long_word, long_word_disp_indirect>},
	 {0x42b0,     7, &_clr<long_word, long_word_index_indirect>},
	 {0x42b8,     0, &_clr<long_word, long_word_abs_short>},
	 {0x42b9,     0, &_clr<long_word, long_word_abs_long>},
	 {0x4400,     7, &_neg<byte, byte_d_register>},
	 {0x4410,     7, &_neg<byte, byte_indirect>},
	 {0x4418,     7, &_neg<byte, byte_postinc_indirect>},
	 {0x4420,     7, &_neg<byte, byte_predec_indirect>},
	 {0x4428,     7, &_neg<byte, byte_disp_indirect>},
	 {0x4430,     7, &_neg<byte, byte_index_indirect>},
	 {0x4438,     0, &_neg<byte, byte_abs_short>},
	 {0x4439,     0, &_neg<byte, byte_abs_long>},
	 {0x4440,     7, &_neg<word, word_d_register>},
	 {0x4450,     7, &_neg<word, word_indirect>},
	 {0x4458,     7, &_neg<word, word_postinc_indirect>},
	 {0x4460,     7, &_neg<word, word_predec_indirect>},
	 {0x4468,     7, &_neg<word, word_disp_indirect>},
	 {0x4470,     7, &_neg<word, word_index_indirect>},
	 {0x4478,     0, &_neg<word, word_abs_short>},
	 {0x4479,     0, &_neg<word, word_abs_long>},
	 {0x4480,     7, &_neg<long_word, long_word_d_register>},
	 {0x4490,     7, &_neg<long_word, long_word_indirect>},
	 {0x4498,     7, &_neg<long_word, long_word_postinc_indirect>},
	 {0x44a0,     7, &_neg<long_word, long_word_predec_indirect>},
	 {0x44a8,     7, &_neg<long_word, long_word_disp_indirect>},
	 {0x44b0,     7, &_neg<long_word, long_word_index_indirect>},
	 {0x44b8,     0, &_neg<long_word, long_word_abs_short>},
	 {0x44b9,     0, &_neg<long_word, long_word_abs_long>},
	 {0x4600,     7, &_not<byte, byte_d_register>},
	 {0x4610,     7, &_not<byte, byte_indirect>},
	 {0x4618,     7, &_not<byte, byte_postinc_indirect>},
	 {0x4620,     7, &_not<byte, byte_predec_indirect>},
	 {0x4628,     7, &_not<byte, byte_disp_indirect>},
	 {0x4630,     7, &_not<byte, byte_index_indirect>},
	 {0x4638,     0, &_not<byte, byte_abs_short>},
	 {0x4639,     0, &_not<byte, byte_abs_long>},
	 {0x4640,     7, &_not<word, word_d_register>},
	 {0x4650,     7, &_not<word, word_indirect>},
	 {0x4658,     7, &_not<word, word_postinc_indirect>},
	 {0x4660,     7, &_not<word, word_predec_indirect>},
	 {0x4668,     7, &_not<word, word_disp_indirect>},
	 {0x4670,     7, &_not<word, word_index_indirect>},
	 {0x4678,     0, &_not<word, word_abs_short>},
	 {0x4679,     0, &_not<word, word_abs_long>},
	 {0x4680,     7, &_not<long_word, long_word_d_register>},
	 {0x4690,     7, &_not<long_word, long_word_indirect>},
	 {0x4698,     7, &_not<long_word, long_word_postinc_indirect>},
	 {0x46a0,     7, &_not<long_word, long_word_predec_indirect>},
	 {0x46a8,     7, &_not<long_word, long_word_disp_indirect>},
	 {0x46b0,     7, &_not<long_word, long_word_index_indirect>},
	 {0x46b8,     0, &_not<long_word, long_word_abs_short>},
	 {0x46b9,     0, &_not<long_word, long_word_abs_long>},
	 {0x46c0,     7, &_move_to_sr<word_d_register>},
	 {0x46d0,     7, &_move_to_sr<word_indirect>},
	 {0x46d8,     7, &_move_to_sr<word_postinc_indirect>},
	 {0x46e0,     7, &_move_to_sr<word_predec_indirect>},
	 {0x46e8,     7, &_move_to_sr<word_disp_indirect>},
	 {0x46f0,     7, &_move_to_sr<word_index_indirect>},
	 {0x46f8,     0, &_move_to_sr<word_abs_short>},
	 {0x46f9,     0, &_move_to_sr<word_abs_long>},
	 {0x46fa,     0, &_move_to_sr<word_disp_pc_indirect>},
	 {0x46fb,     0, &_move_to_sr<word_index_pc_indirect>},
	 {0x46fc,     0, &_move_to_sr<word_immediate>},
	 {0x4840,     7, &_swap},
	 {0x4850,     7, &_pea<word_indirect>},
	 {0x4868,     7, &_pea<word_disp_indirect>},
	 {0x4870,     7, &_pea<word_index_indirect>},
	 {0x4878,     0, &_pea<word_abs_short>},
	 {0x4879,     0, &_pea<word_abs_long>},
	 {0x487a,     0, &_pea<word_disp_pc_indirect>},
	 {0x487b,     0, &_pea<word_index_pc_indirect>},
	 {0x4880,     7, &_ext<byte, word>},
	 {0x4890,     7, &_movem_r_m<word, word_indirect>},
	 {0x48a0,     7, &_movem_r_predec<word>},
	 {0x48a8,     7, &_movem_r_m<word, word_disp_indirect>},
	 {0x48b0,     7, &_movem_r_m<word, word_index_indirect>},
	 {0x48b8,     0, &_movem_r_m<word, word_abs_short>},
	 {0x48b9,     0, &_movem_r_m<word, word_abs_long>},
	 {0x48c0,     7, &_ext<word, long_word>},
	 {0x48d0,     7, &_movem_r_m<long_word, long_word_indirect>},
	 {0x48e0,     7, &_movem_r_predec<long_word>},
	 {0x48e8,     7, &_movem_r_m<long_word, long_word_disp_indirect>},
	 {0x48f0,     7, &_movem_r_m<long_word, long_word_index_indirect>},
	 {0x48f8,     0, &_movem_r_m<long_word, long_word_abs_short>},
	 {0x48f9,     0, &_movem_r_m<long_word, long_word_abs_long>},
	 {0x4a00,     7, &_tst<byte, byte_d_register>},
	 {0x4a10,     7, &_tst<byte, byte_indirect>},
	 {0x4a18,     7, &_tst<byte, byte_postinc_indirect>},
	 {0x4a20,     7, &_tst<byte, byte_predec_indirect>},
	 {0x4a28,     7, &_tst<byte, byte_disp_indirect>},
	 {0x4a30,     7, &_tst<byte, byte_index_indirect>},
	 {0x4a38,     0, &_tst<byte, byte_abs_short>},
	 {0x4a39,     0, &_tst<byte, byte_abs_long>},
	 {0x4a40,     7, &_tst<word, word_d_register>},
	 {0x4a50,     7, &_tst<word, word_indirect>},
	 {0x4a58,     7, &_tst<word, word_postinc_indirect>},
	 {0x4a60,     7, &_tst<word, word_predec_indirect>},
	 {0x4a68,     7, &_tst<word, word_disp_indirect>},
	 {0x4a70,     7, &_tst<word, word_index_indirect>},
	 {0x4a78,     0, &_tst<word, word_abs_short>},
	 {0x4a79,     0, &_tst<word, word_abs_long>},
	 {0x4a80,     7, &_tst<long_word, long_word_d_register>},
	 {0x4a90,     7, &_tst<long_word, long_word_indirect>},
	 {0x4a98,     7, &_tst<long_word, long_word_postinc_indirect>},
	 {0x4aa0,     7, &_tst<long_word, long_word_predec_indirect>},
	 {0x4aa8,     7, &_tst<long_word, long_word_disp_indirect>},
	 {0x4ab0,     7, &_tst<long_word, long_word_index_indirect>},
	 {0x4ab8,     0, &_tst<long_word, long_word_abs_short>},
	 {0x4ab9,     0, &_tst<long_word, long_word_abs_long>},
	 {0x4c90,     7, &_movem_m_r<word, word_indirect>},
	 {0x4c98,     7, &_movem_postinc_r<word>},
	 {0x4ca8,     7, &_movem_m_r<word, word_disp_indirect>},
	 {0x4cb0,     7, &_movem_m_r<word, word_index_indirect>},
	 {0x4cb8,     0, &_movem_m_r<word, word_abs_short>},
	 {0x4cb9,     0, &_movem_m_r<word, word_abs_long>},
	 {0x4cba,     0, &_movem_m_r<word, word_disp_pc_indirect>},
	 {0x4cbb,     0, &_movem_m_r<word, word_index_pc_indirect>},
	 {0x4cd0,     7, &_movem_m_r<long_word, long_word_indirect>},
	 {0x4cd8,     7, &_movem_postinc_r<long_word>},
	 {0x4ce8,     7, &_movem_m_r<long_word, long_word_disp_indirect>},
	 {0x4cf0,     7, &_movem_m_r<long_word, long_word_index_indirect>},
	 {0x4cf8,     0, &_movem_m_r<long_word, long_word_abs_short>},
	 {0x4cf9,     0, &_movem_m_r<long_word, long_word_abs_long>},
	 {0x4cfa,     0, &_movem_m_r<long_word, long_word_disp_pc_indirect>},
	 {0x4cfb,     0, &_movem_m_r<long_word, long_word_index_pc_indirect>},
	 {0x4e50,     7, &_link},
	 {0x4e58,     7, &_unlk},
	 {0x4e60,     7, &_move_to_usp},
	 {0x4e68,     7, &_move_from_usp},
	 {0x4e71,     0, &_nop},
	 {0x4e73,     0, &_rte},
	 {0x4e75,     0, &_rts},
	 {0x4e90,     7, &_jsr<word_indirect>},
	 {0x4ea8,     7, &_jsr<word_disp_indirect>},
	 {0x4eb0,     7, &_jsr<word_index_indirect>},
	 {0x4eb8,     0, &_jsr<word_abs_short>},
	 {0x4eb9,     0, &_jsr<word_abs_long>},
	 {0x4eba,     0, &_jsr<word_disp_pc_indirect>},
	 {0x4ebb,     0, &_jsr<word_index_pc_indirect>},
	 {0x4ed0,     7, &_jmp<word_indirect>},
	 {0x4ee8,     7, &_jmp<word_disp_indirect>},
	 {0x4ef0,     7, &_jmp<word_index_indirect>},
	 {0x4ef8,     0, &_jmp<word_abs_short>},
	 {0x4ef9,     0, &_jmp<word_abs_long>},
	 {0x4efa,     0, &_jmp<word_disp_pc_indirect>},
	 {0x4efb,     0, &_jmp<word_index_pc_indirect>}};

    for (const instruction_map *i = inst + 0;
	 i != inst + sizeof inst / sizeof inst[0]; ++i)
      p.set_instruction(i->base, i->mask, make_pair(i->handler, data));
  }
}
