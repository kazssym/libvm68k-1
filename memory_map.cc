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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307
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

#include <algorithm>

namespace vm68k
{
  using namespace std;

  uint16_type
  memory_map::get_16(uint32_type address, function_code fc) const
    throw (memory_exception)
  {
    if ((address & 1) != 0)
      throw address_error(address, memory::READ | fc);

    return this->get_16_unchecked(address, fc);
  }

  uint32_type
  memory_map::get_32(uint32_type address, function_code fc) const
    throw (memory_exception)
  {
    if ((address & 1) != 0)
      throw address_error(address, memory::READ | fc);

    uint32_type value;
    if ((address >> 1 & 1) != 0)
      {
	value = uint32_type(this->get_16_unchecked(address, fc)) << 16;
	value |= this->get_16_unchecked(address + 2, fc) & 0xffff;
      }
    else
      {
	const memory *p = *this->find_memory(address);
	value = p->get_32(address, fc);
      }

    return value;
  }

  string
  memory_map::get_string(uint32_type address, function_code fc) const
  {
    string s;
    for (;;)
      {
	int c = this->get_8(address++, fc);
	if (c == 0)
	  break;
	s += c;
      }

    return s;
  }

  /* Read a block of data from memory.  */
  void
  memory_map::read(uint32_type address, void *data, size_t size,
		   function_code fc) const
  {
    unsigned char *i = static_cast<unsigned char *>(data);
    unsigned char *last = i + size;

    while (i != last)
      *i++ = this->get_8(address++, fc);
  }

  void
  memory_map::put_16(uint32_type address, uint16_type value, function_code fc)
    throw (memory_exception)
  {
    if ((address & 1) != 0)
      throw address_error(address, memory::WRITE | fc);

    this->put_16_unchecked(address, value, fc);
  }

  void
  memory_map::put_32(uint32_type address, uint32_type value, function_code fc)
    throw (memory_exception)
  {
    if ((address & 1) != 0)
      throw address_error(address, memory::WRITE | fc);

    if ((address >> 1 & 1) != 0)
      {
	this->put_16_unchecked(address,     value >> 16, fc);
	this->put_16_unchecked(address + 2, value,       fc);
      }
    else
      {
	memory *p = *this->find_memory(address);
	p->put_32(address, value, fc);
      }
  }

  void
  memory_map::put_string(uint32_type address, const string &s,
			 function_code fc)
  {
    for (string::const_iterator i = s.begin();
	 i != s.end();
	 ++i)
      this->put_8(address++, *i, fc);

    this->put_8(address++, 0, fc);
  }

  /* Write a block of data to memory.  */
  void
  memory_map::write(uint32_type address, const void *data, size_t size,
		    function_code fc)
  {
    const unsigned char *i = static_cast<const unsigned char *>(data);
    const unsigned char *last = i + size;

    while (i != last)
      this->put_8(address++, *i++, fc);
  }
  
  void
  memory_map::fill(uint32_type first, uint32_type last, memory *p)
  {
    vector<memory *>::iterator i = this->find_memory(last + PAGE_SIZE - 1);
    if (i == page_table.begin())
      i = page_table.end();
    std::fill(this->find_memory(first), i, p);
  }
  
  namespace
  {
    /* Default memory that always raises a bus error.  */
    class missing_memory: public memory
    {
    public:
      int get_8(uint32_type address, function_code) const
	throw (memory_exception);
      uint16_type get_16(uint32_type address, function_code) const
	throw (memory_exception);

      void put_8(uint32_type address, int, function_code)
	throw (memory_exception);
      void put_16(uint32_type address, uint16_type, function_code)
	throw (memory_exception);
    };

    int
    missing_memory::get_8(uint32_type address, function_code fc) const
      throw (memory_exception)
    {
      throw bus_error(address, READ | fc);
    }

    uint16_type
    missing_memory::get_16(uint32_type address, function_code fc) const
      throw (memory_exception)
    {
      assert((address & 1) == 0);
      throw bus_error(address, READ | fc);
    }

    void
    missing_memory::put_8(uint32_type address, int value, function_code fc)
      throw (memory_exception)
    {
      throw bus_error(address, WRITE | fc);
    }

    void
    missing_memory::put_16(uint32_type address, uint16_type value,
			   function_code fc)
      throw (memory_exception)
    {
      assert((address & 1) == 0);
      throw bus_error(address, WRITE | fc);
    }

    missing_memory __null_memory;
  }

  memory *
  memory_map::null_memory() throw ()
  {
    return &__null_memory;
  }

  memory_map::~memory_map()
  {
  }

  memory_map::memory_map()
    : page_table(NPAGES, null_memory())
  {
  }
}
