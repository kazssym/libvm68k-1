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
#define _POSIX_C_SOURCE 199506L	// We want POSIX.1c if not GNU.

#include <vm68k/bus>
#include <algorithm>

#ifdef HAVE_NANA_H
# include <nana.h>
#else
# include <cassert>
# define I assert
#endif

namespace vm68k
{
  using namespace std;

  uint16_type
  bus::get_16(uint32_type address, function_code fc) const
    throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, READ | fc);

    return get_16_aligned(address, fc);
  }

  uint32_type
  bus::get_32(uint32_type address, function_code fc) const
    throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, READ | fc);

    uint32_type value;
    if ((address >> 1 & 1) != 0)
      {
	value = uint32_type(get_16_aligned(address, fc)) << 16;
	value |= get_16_aligned(address + 2, fc) & 0xffff;
      }
    else
      {
	const bus_target *p = *find_memory(address);
	value = p->get_32(address, fc);
      }

    return value;
  }

  string
  bus::get_string(uint32_type address, function_code fc) const
  {
    string s;
    for (;;)
      {
	int c = get_8(address++, fc);
	if (c == 0)
	  break;
	s += c;
      }

    return s;
  }

  /* Read a block of data from memory.  */
  void
  bus::read(uint32_type address, void *data, size_t size,
		   function_code fc) const
  {
    unsigned char *i = static_cast<unsigned char *>(data);
    unsigned char *last = i + size;

    while (i != last)
      *i++ = get_8(address++, fc);
  }

  void
  bus::put_16(uint32_type address, uint16_type value, function_code fc)
    throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, WRITE | fc);

    put_16_aligned(address, value, fc);
  }

  void
  bus::put_32(uint32_type address, uint32_type value, function_code fc)
    throw (bus_error, address_error)
  {
    if ((address & 1) != 0)
      throw address_error(address, WRITE | fc);

    if ((address >> 1 & 1) != 0)
      {
	put_16_aligned(address,     value >> 16, fc);
	put_16_aligned(address + 2, value,       fc);
      }
    else
      {
	bus_target *p = *find_memory(address);
	p->put_32(address, value, fc);
      }
  }

  void
  bus::put_string(uint32_type address, const string &s,
			 function_code fc)
  {
    for (string::const_iterator i = s.begin();
	 i != s.end();
	 ++i)
      put_8(address++, *i, fc);

    put_8(address++, 0, fc);
  }

  /* Write a block of data to memory.  */
  void
  bus::write(uint32_type address, const void *data, size_t size,
		    function_code fc)
  {
    const unsigned char *i = static_cast<const unsigned char *>(data);
    const unsigned char *last = i + size;

    while (i != last)
      put_8(address++, *i++, fc);
  }

  void
  bus::fill_memory(uint32_type first, uint32_type last, bus_target *t)
  {
    vector<bus_target *>::iterator i = find_memory(last + PAGE_SIZE - 1);
    if (i == page_table.begin())
      i = page_table.end();

    std::fill(find_memory(first), i, t);
  }
}

namespace vm68k
{
  namespace
  {
    /* Default memory that always raises a bus error.  */
    class no_target: public bus_target
    {
    public:
      int get_8(uint32_type address, function_code) const
	throw (bus_error);
      uint16_type get_16(uint32_type address, function_code) const
	throw (bus_error);

      void put_8(uint32_type address, int, function_code)
	throw (bus_error);
      void put_16(uint32_type address, uint16_type, function_code)
	throw (bus_error);
    };

    int
    no_target::get_8(uint32_type address, function_code fc) const
      throw (bus_error)
    {
      throw bus_error(address, READ | fc);
    }

    uint16_type
    no_target::get_16(uint32_type address, function_code fc) const
      throw (bus_error)
    {
      I((address & 1) == 0);
      throw bus_error(address, READ | fc);
    }

    void
    no_target::put_8(uint32_type address, int value, function_code fc)
      throw (bus_error)
    {
      throw bus_error(address, WRITE | fc);
    }

    void
    no_target::put_16(uint32_type address, uint16_type value, function_code fc)
      throw (bus_error)
    {
      I((address & 1) == 0);
      throw bus_error(address, WRITE | fc);
    }
  }

  bus_target *
  bus::no_target() throw ()
  {
    static class no_target t;
    return &t;
  }

  bus::~bus()
  {
  }

  bus::bus()
    : page_table(NPAGES, no_target())
  {
  }
}
