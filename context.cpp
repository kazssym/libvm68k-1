/* Virtual M68000 Toolkit
   Copyright (C) 1998-2008 Hypercore Software Design, Ltd.

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
#include <config.h>
#endif

#if __GNUC__
#define _VM68K_PUBLIC __attribute__ ((__visibility__ ("default")))
#endif

#include <vm68k/context>
#include <cassert>
#include <pthread.h>

namespace vx68k_m68k
{
  context *context::current_context ()
  {
    return NULL;
  }

  context::context (system_bus *bus)
  {
    _bus = bus;
    if (this->super ())
      {
        dfc_cache = SUPER_DATA;
        pfc_cache = SUPER_PROGRAM;
      }
    else
      {
        dfc_cache = USER_DATA;
        pfc_cache = USER_PROGRAM;
      }

    a_interrupted = false;
  }

  void context::set_super (bool state)
  {
    if (state != super ())
      {
        if (state)
          {
	    _named_reg.usp = _named_reg.sp;
	    _status.set_s_bit (true);
	    _named_reg.sp = _named_reg.ssp;

	    dfc_cache = SUPER_DATA;
	    pfc_cache = SUPER_PROGRAM;
	  }
        else
          {
	    _named_reg.ssp = _named_reg.sp;
	    _status.set_s_bit (false);
	    _named_reg.sp = _named_reg.usp;

	    dfc_cache = USER_DATA;
	    pfc_cache = USER_PROGRAM;
	  }
      }
  }

  void context::set_status (udata_fast16_t value)
  {
    set_super ((value & 0x2000) != 0);
    _status = value;
  }

  void context::interrupt (int priority, udata_fast8_t vecno)
  {
    if (priority < 1 || priority > 7)
      return;

    vecno &= 0xffU;
    interrupt_queue[7 - priority].push (vecno);
    a_interrupted = true;
  }

#if 0
  address_t
  context::handle_interrupts (address_t pc)
  {
    using std::vector;
    using std::queue;

    vector<queue<unsigned int> >::iterator i = interrupt_queues.begin();
    while (i->empty())
      {
	++i;
	assert (i != interrupt_queues.end());
      }

    int prio = interrupt_queues.end() - i;
    int level = sr() >> 8 & 7;
    if (prio == 7 || prio > level)
      {
	unsigned int vecno = i->front();
	i->pop();

	udata_fast16_t old_sr = status ();
	set_status (old_sr & ~0x700 | prio << 8);
	set_super (true);
	_named_reg.sp -= 6;
	_bus->put32 (SUPER_DATA, _named_reg.sp + 2, pc);
	_bus->put16 (SUPER_DATA, _named_reg.sp + 0, old_sr);

	address_t address = vecno * 4u;
	pc = _bus->get32 (SUPER_DATA, address);

	a_interrupted = false;
	vector<queue<unsigned int> >::iterator j = i;
	while (j != interrupt_queues.end())
	  {
	    if (!j->empty())
	      a_interrupted = true;
	    ++j;
	  }
      }

    return pc;
  }
#endif
}
