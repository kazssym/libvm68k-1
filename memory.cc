/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2002 Hypercore Software Design, Ltd.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307
   USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#undef const
#undef inline

#define _GNU_SOURCE 1
#define _POSIX_C_SOURCE 199506L	// POSIX.1c

#include <vm68k/bus>
#include <algorithm>
#include <cassert>

using namespace std;

namespace vm68k
{
  uint16_type
  memory::load_16(uint32_type address,
		  function_code fc) const throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, READ | fc);

    return load_16_aligned(address, fc);
  }

  uint32_type
  memory::load_32(uint32_type address,
		  function_code fc) const throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, READ | fc);

    uint32_type value;
    if ((address >> 1 & 1) != 0)
      {
	value = uint32_type(load_16_aligned(address, fc)) << 16;
	value |= load_16_aligned(address + 2, fc) & 0xffff;
      }
    else
      {
	const memory_device *d = *find_device(address);
	value = d->load_32(address, fc);
      }

    return value;
  }

  string
  memory::load_string(uint32_type address,
		      function_code fc) const throw (bus_error)
  {
    string s;
    for (;;)
      {
	int c = load_8(address++, fc);
	if (c == 0)
	  break;
	s += c;
      }

    return s;
  }
}

namespace vm68k
{
  void
  memory::store_16(uint32_type address, uint16_type value,
		   function_code fc) throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, WRITE | fc);

    store_16_aligned(address, value, fc);
  }

  void
  memory::store_32(uint32_type address, uint32_type value,
		   function_code fc) throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, WRITE | fc);

    if ((address >> 1 & 1) != 0)
      {
	store_16_aligned(address,     value >> 16, fc);
	store_16_aligned(address + 2, value,       fc);
      }
    else
      {
	memory_device *d = *find_device(address);
	d->store_32(address, value, fc);
      }
  }

  void
  memory::store_string(uint32_type address, const string &s,
		       function_code fc) throw (bus_error)
  {
    for (string::const_iterator i = s.begin();
	 i != s.end();
	 ++i)
      store_8(address++, *i, fc);

    store_8(address++, 0, fc);
  }
}

namespace vm68k
{
  /* Read a block of data from memory.  */
  void
  memory::copy_out(uint32_type address, void *data, size_t size,
		   function_code fc) const throw (bus_error)
  {
    unsigned char *i = static_cast<unsigned char *>(data);
    unsigned char *last = i + size;

    while (i != last)
      *i++ = load_8(address++, fc);
  }

  /* Write a block of data to memory.  */
  void
  memory::copy_in(uint32_type address, const void *data, size_t size,
		  function_code fc) throw (bus_error)
  {
    const unsigned char *i = static_cast<const unsigned char *>(data);
    const unsigned char *last = i + size;

    while (i != last)
      store_8(address++, *i++, fc);
  }
}

namespace vm68k
{
  namespace
  {
    /* Default memory that always raises a bus error.  */
    class bus_error_device: public memory_device
    {
    public:
      int load_8(uint32_type address,
		 function_code) const throw (bus_error);
      uint16_type load_16(uint32_type address,
			  function_code) const throw (bus_error);
      void store_8(uint32_type address, int,
		   function_code) throw (bus_error);
      void store_16(uint32_type address, uint16_type,
		    function_code) throw (bus_error);
    };

    int
    bus_error_device::load_8(uint32_type address,
			     function_code fc) const throw (bus_error)
    {
      throw bus_error(address, READ | fc);
    }

    uint16_type
    bus_error_device::load_16(uint32_type address,
			      function_code fc) const throw (bus_error)
    {
      assert((address & 1) == 0);
      throw bus_error(address, READ | fc);
    }

    void
    bus_error_device::store_8(uint32_type address, int value,
			      function_code fc) throw (bus_error)
    {
      throw bus_error(address, WRITE | fc);
    }

    void
    bus_error_device::store_16(uint32_type address, uint16_type value,
			       function_code fc) throw (bus_error)
    {
      assert((address & 1) == 0);
      throw bus_error(address, WRITE | fc);
    }

    static bus_error_device no_device;
  }

  memory::memory()
    : page_table(address_max() / page_size() + 1, &no_device)
  {
  }

  void
  memory::add(uint32_type first, uint32_type last, memory_device *d)
  {
    if (d == 0)
      d = &no_device;

    vector<memory_device *>::iterator j = find_device(last + page_size() - 1);
    if (j == page_table.begin())
      j = page_table.end();

    fill(find_device(first), j, d);
  }

  memory::~memory()
  {
  }
}
