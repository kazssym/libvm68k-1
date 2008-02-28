/* Virtual M68000 Toolkit
   Copyright (C) 1998-2008 Hypercore Software Design, Ltd.

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

#include <vm68k/bus>

#include <algorithm>
#include <cassert>

using std::string;
using std::fill;

namespace vx68k_m68k
{
  bus_error::bus_error (udata_fast16_t status, address_t address)
    throw ()
  {
    assert ((status & ~0xffffU) == 0);
    _ssw = status;
    _address = address;
  }

  const char *bus_error::what () const
    throw ()
  {
    return "vx68k_m68k::bus_error";
  }

  address_error::address_error (udata_fast16_t status, address_t address)
    throw ()
  {
    assert ((status & ~0xffffU) == 0);
    _ssw = status;
    _address = status;
  }

  const char *address_error::what () const
    throw ()
  {
    return "vx68k_m68k::address_error";
  }

  /* Class accessible etc. implementation.  */

  accessible::~accessible ()
  {
  }

  udata_fast8_t accessible::read8 (function_code fc, address_t address) const
    throw (bus_error)
  {
    throw bus_error (READ | fc, address);
  }

  udata_fast16_t accessible::read16 (function_code fc, address_t address) const
    throw (bus_error)
  {
    assert ((address & 1U) == 0);
    throw bus_error (READ | fc, address);
  }

  udata_fast32_t accessible::read32 (function_code fc, address_t address) const
    throw (bus_error)
  {
    assert ((address & 3U) == 0);
    udata_fast32_t value = ((udata_fast32_t) this->read16 (fc, address)) << 16;
    value |= read16 (fc, address + 2) & 0xffffU;
    return value;
  }

  void accessible::write8 (function_code fc, address_t address,
                           udata_fast8_t value)
    throw (bus_error)
  {
    throw bus_error (WRITE | fc, address);
  }

  void accessible::write16 (function_code fc, address_t address,
                            udata_fast16_t value)
    throw (bus_error)
  {
    assert ((address & 1U) == 0);
    throw bus_error (WRITE | fc, address);
  }

  void accessible::write32 (function_code fc, address_t address,
                            udata_fast32_t value)
    throw (bus_error)
  {
    assert ((address & 3U) == 0);
    this->write16 (fc, address,     value >> 16);
    this->write16 (fc, address + 2, value);
  }

  /* Class bus implementation.  */

  system_bus::system_bus ()
  {
    page_table[USER_DATA]    .assign (NPAGES, &null_accessible);
    page_table[USER_PROGRAM] .assign (NPAGES, &null_accessible);
    page_table[SUPER_DATA]   .assign (NPAGES, &null_accessible);
    page_table[SUPER_PROGRAM].assign (NPAGES, &null_accessible);
  }

  system_bus::~system_bus ()
  {
  }

  void system_bus::map_page (int bits,
                             address_t address, udata_fast32_t size,
                             accessible *p)
  {
    for (int fc = 0; fc != 7; bits >>= 1, ++fc)
      {
        if ((bits & 1U) != 0 && !page_table[fc].empty ())
          {
            page_table_type::iterator i =
              this->find_page ((function_code) fc,
                               address + size + PAGE_SIZE - 1);
            if (i == page_table[fc].begin ())
              {
                i = page_table[fc].end ();
              }

            fill (this->find_page ((function_code) fc, address), i, p);
          }
      }
  }

  void system_bus::unmap_page (int bits,
                               address_t address, udata_fast32_t size)
  {
    this->map_page (bits, address, size, &null_accessible);
  }

  udata_fast16_t system_bus::read16 (function_code fc, address_t address) const
    throw (bus_error, address_error)
  {
    if ((address & 1U) != 0)
      throw address_error (READ | fc, address);

    return this->read16_unchecked (fc, address);
  }

  udata_fast32_t system_bus::read32 (function_code fc, address_t address) const
    throw (bus_error, address_error)
  {
    if ((address & 1U) != 0)
      throw address_error (READ | fc, address);

    udata_fast32_t value;
    if ((address & 2U) != 0)
      {
	value = (udata_fast32_t) this->read16_unchecked (fc, address) << 16;
	value |= this->read16_unchecked (fc, address + 2) & 0xffff;
      }
    else
      {
	const accessible *p = *(this->find_page (fc, address));
	value = p->read32 (fc, address);
      }

    return value;
  }

  string system_bus::read_string (function_code fc, address_t address) const
  {
    string s;
    for (;;)
      {
	udata_fast8_t c = this->read8 (fc, address++);
	if (c == 0)
	  break;
	s += (char) c;
      }

    return s;
  }

  /* Read a block of data from memory.  */
  void system_bus::read (function_code fc, address_t address,
                         void *data, size_t size) const
  {
    unsigned char *i = static_cast<unsigned char *> (data);
    unsigned char *last = i + size;

    while (i != last)
      {
        *i++ = this->read8 (fc, address++);
      }
  }

  void system_bus::write16 (function_code fc, address_t address,
                            udata_fast16_t value)
    throw (bus_error, address_error)
  {
    if ((address & 1U) != 0)
      throw address_error (WRITE | fc, address);

    this->write16_unchecked (fc, address, value);
  }

  void system_bus::write32 (function_code fc, address_t address,
                            udata_fast32_t value)
    throw (bus_error, address_error)
  {
    if ((address & 1U) != 0)
      throw address_error (WRITE | fc, address);

    if ((address & 2U) != 0)
      {
	this->write16_unchecked (fc, address,     value >> 16);
	this->write16_unchecked (fc, address + 2, value);
      }
    else
      {
	accessible *p = *(this->find_page (fc, address));
	p->write32 (fc, address, value);
      }
  }

  void system_bus::write_string (function_code fc, address_t address,
                                 const string &s)
  {
    for (string::const_iterator i = s.begin(); i != s.end(); ++i)
      {
        this->write8 (fc, address++, *i);
      }

    this->write8 (fc, address++, 0);
  }

  /* Write a block of data to memory.  */
  void system_bus::write (function_code fc, address_t address,
                          const void *data, size_t size)
  {
    const unsigned char *i = static_cast<const unsigned char *> (data);
    const unsigned char *last = i + size;

    while (i != last)
      {
        this->write8 (fc, address++, *i++);
      }
  }
}
