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

#include <vm68k/processor>
#include <vm68k/size>
#include <algorithm>
#include <cstdio>

#ifdef HAVE_NANA_H
# include <nana.h>
#else
# include <cassert>
# define I assert
#endif

using namespace std;

#ifdef HAVE_NANA_H
bool nana_instruction_trace = false;
#endif

namespace vm68k
{
  uint32_type
  processor::run(uint32_type pc, context &c) const
    throw (processor_exception)
  {
    try
      {
	for (;;)
	  {
	    if (c.interrupted())
	      pc = c.handle_interrupts(pc);

#ifdef LG
# ifdef DUMP_REGISTERS
	    for (unsigned int i = 0; i != 8; ++i)
	      {
		LG(nana_instruction_trace,
		   "| %%d%u = 0x%08lx, %%a%u = 0x%08lx\n",
		   i, (unsigned long) c.regs.d[i],
		   i, (unsigned long) c.regs.a[i]);
	      }
# endif
	    LG(nana_instruction_trace, "| 0x%08lx (0x%04x)\n",
	       long_word::normal_u(pc) + 0UL, c.fetch_u(word(), pc));
#endif
	    pc = dispatch(pc, c, c.fetch_u(word(), pc));
	  }
      }
    catch (const bus_error &e)
      {
	throw bus_error_exception(pc, e);
      }
    catch (const address_error &e)
      {
	throw address_error_exception(pc, e);
      }
  }

  void
  processor::set_instruction(int code, int mask, const instruction_type &in)
  {
    I (code >= 0);
    I (code < 0x10000);
    code &= ~mask;
    for (int i = code; i <= (code | mask); ++i)
      {
	if ((i & ~mask) == code)
	  {
	    instruction_type old_value = set_instruction(i, in);
#ifdef L
	    if (old_value.first != &illegal)
	      L("warning: Replacing instruction handler at 0x%04x\n", i);
#endif
	  }
      }
  }
  
  processor::processor()
    : instructions(0x10000, instruction_type(&illegal, 0))
  {
    install_instructions_0(*this, 0);
    install_instructions_1(*this, 0);
    install_instructions_2(*this, 0);
    install_instructions_3(*this, 0);
    install_instructions_4(*this, 0);
    install_instructions_5(*this, 0);
    install_instructions_6(*this, 0);
    install_instructions_7(*this, 0);
    install_instructions_8(*this, 0);
    install_instructions_9(*this, 0);
    install_instructions_11(*this, 0);
    install_instructions_12(*this, 0);
    install_instructions_13(*this, 0);
    install_instructions_14(*this, 0);
  }
  
  /* Executes an illegal instruction.  */
  uint32_type
  processor::illegal(uint32_type pc, context &, uint16_type, void *)
  {
    throw illegal_instruction_exception(pc);
  }
}
