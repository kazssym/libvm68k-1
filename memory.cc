/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2001 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const			// C++ must have `const'.

#define _GNU_SOURCE 1
#define _POSIX_C_SOURCE 199506L	// We want POSIX.1c if not GNU.

#ifdef HAVE_NANA_H
# include <nana.h>
#else
# include <cassert>
# define I assert
#endif

#include "vm68k/memory.h"

namespace vm68k
{
  using namespace std;

  int
  bus_error::vecno() throw ()
  {
    return 2;
  }

  const char *
  bus_error::what() const throw ()
  {
    return "vm68k::bus_error";
  }

  int
  address_error::vecno() throw ()
  {
    return 3;
  }

  const char *
  address_error::what() const throw ()
  {
    return "vm68k::address_error";
  }
  
  uint32_type
  memory::get_32(uint32_type address, function_code fc) const
    throw (memory_error)
  {
    I((address & 3) == 0);
    uint32_type value = uint32_type(get_16(address, fc)) << 16;
    value |= get_16(address + 2, fc) & 0xffff;
    return value;
  }

  void
  memory::put_32(uint32_type address, uint32_type value,
		 function_code fc) throw (memory_error)
  {
    I((address & 3) == 0);
    put_16(address,     value >> 16, fc);
    put_16(address + 2, value,       fc);
  }
}
