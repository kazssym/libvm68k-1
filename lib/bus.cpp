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

namespace vx68k
{
  vm68k_bus_error::vm68k_bus_error (uint_fast16_t status,
                                    vm68k_address_t addr)
    throw ()
  {
    assert ((status & ~0xffffU) == 0);
    _status = status;
    _address = addr;
  }

  const char *vm68k_bus_error::what () const throw ()
  {
    return "vm68k_bus_error";
  }

  vm68k_address_error::vm68k_address_error (uint_fast16_t status,
                                            vm68k_address_t addr)
    throw ()
  {
    assert ((status & ~0xffffU) == 0);
    _status = status;
    _address = addr;
  }

  const char *vm68k_address_error::what () const throw ()
  {
    return "vm68k_address_error";
  }

  /* Class accessible etc. implementation.  */

  vm68k_bus::mappable::~mappable ()
  {
  }

  uint_fast8_t vm68k_bus::mappable::read8 (function_code func,
                                           vm68k_address_t addr) const
    throw (vm68k_bus_error)
  {
    throw vm68k_bus_error (READ | func, addr);
  }

  uint_fast16_t vm68k_bus::mappable::read16 (function_code func,
                                             vm68k_address_t addr) const
    throw (vm68k_bus_error)
  {
    assert ((addr & 1U) == 0);
    throw vm68k_bus_error (READ | func, addr);
  }

  uint_fast32_t vm68k_bus::mappable::read32 (function_code func,
                                             vm68k_address_t addr) const
    throw (vm68k_bus_error)
  {
    assert ((addr & 3U) == 0);
    uint_fast32_t value =
      ((uint_fast32_t) this->read16 (func, addr)) << 16;
    value |= read16 (func, addr + 2) & 0xffffU;
    return value;
  }

  void vm68k_bus::mappable::write8 (function_code func, vm68k_address_t addr,
                                    uint_fast8_t value)
    throw (vm68k_bus_error)
  {
    throw vm68k_bus_error (WRITE | func, addr);
  }

  void vm68k_bus::mappable::write16 (function_code func, vm68k_address_t addr,
                                     uint_fast16_t value)
    throw (vm68k_bus_error)
  {
    assert ((addr & 1U) == 0);
    throw vm68k_bus_error (WRITE | func, addr);
  }

  void vm68k_bus::mappable::write32 (function_code func, vm68k_address_t addr,
                                     uint_fast32_t value)
    throw (vm68k_bus_error)
  {
    assert ((addr & 3U) == 0);
    this->write16 (func, addr,     value >> 16);
    this->write16 (func, addr + 2, value);
  }

  /* Class bus implementation.  */

  vm68k_bus::vm68k_bus ()
  {
    page_table[USER_DATA]    .assign (NPAGES, &null_accessible);
    page_table[USER_PROGRAM] .assign (NPAGES, &null_accessible);
    page_table[SUPER_DATA]   .assign (NPAGES, &null_accessible);
    page_table[SUPER_PROGRAM].assign (NPAGES, &null_accessible);
  }

  vm68k_bus::~vm68k_bus ()
  {
  }

  void vm68k_bus::map_pages (int func_mask, vm68k_address_t addr,
                             uint_fast32_t size, mappable *p)
  {
    for (int func = 0; func != 7; func_mask >>= 1, ++func)
      {
        if ((func_mask & 1U) != 0 && !page_table[func].empty ())
          {
            page_table_type::iterator i =
              this->find_page ((function_code) func,
                               addr + size + PAGE_SIZE - 1);
            if (i == page_table[func].begin ())
              {
                i = page_table[func].end ();
              }

            fill (this->find_page ((function_code) func, addr), i, p);
          }
      }
  }

  void vm68k_bus::unmap_pages (int func_mask, vm68k_address_t addr,
                               uint_fast32_t size)
  {
    this->map_pages (func_mask, addr, size, &null_accessible);
  }

  uint_fast16_t vm68k_bus::read16 (function_code func,
                                   vm68k_address_t addr) const
    throw (vm68k_bus_error, vm68k_address_error)
  {
    if ((addr & 1U) != 0)
      {
        throw vm68k_address_error (READ | func, addr);
      }

    return this->read16_unchecked (func, addr);
  }

  uint_fast32_t vm68k_bus::read32 (function_code func,
                                   vm68k_address_t addr) const
    throw (vm68k_bus_error, vm68k_address_error)
  {
    if ((addr & 1U) != 0)
      {
        throw vm68k_address_error (READ | func, addr);
      }

    uint_fast32_t value;
    if ((addr & 2U) != 0)
      {
        value =
          ((uint_fast32_t) this->read16_unchecked (func, addr)) << 16;
        value |= this->read16_unchecked (func, addr + 2) & 0xffff;
      }
    else
      {
        const mappable *p = *(this->find_page (func, addr));
        value = p->read32 (func, addr);
      }

    return value;
  }

  string vm68k_bus::read_string (function_code func,
                                 vm68k_address_t addr) const
  {
    string s;
    for (;;)
      {
        uint_fast8_t c = this->read8 (func, addr++);
        if (c == 0)
          {
            break;
          }

        s += (char) c;
      }

    return s;
  }

  /* Read a block of data from memory.  */
  void vm68k_bus::read (function_code func, vm68k_address_t addr, void *data,
                        size_t size) const
  {
    unsigned char *i = static_cast<unsigned char *> (data);
    unsigned char *last = i + size;

    while (i != last)
      {
        *i++ = this->read8 (func, addr++);
      }
  }

  void vm68k_bus::write16 (function_code func, vm68k_address_t addr,
                           uint_fast16_t value)
    throw (vm68k_bus_error, vm68k_address_error)
  {
    if ((addr & 1U) != 0)
      {
        throw vm68k_address_error (WRITE | func, addr);
      }

    this->write16_unchecked (func, addr, value);
  }

  void vm68k_bus::write32 (function_code func, vm68k_address_t addr,
                           uint_fast32_t value)
    throw (vm68k_bus_error, vm68k_address_error)
  {
    if ((addr & 1U) != 0)
      {
        throw vm68k_address_error (WRITE | func, addr);
      }

    if ((addr & 2U) != 0)
      {
        this->write16_unchecked (func, addr,     value >> 16);
        this->write16_unchecked (func, addr + 2, value);
      }
    else
      {
        mappable *p = *(this->find_page (func, addr));
        p->write32 (func, addr, value);
      }
  }

  void vm68k_bus::write_string (function_code func, vm68k_address_t addr,
                                const string &s)
  {
    for (string::const_iterator i = s.begin(); i != s.end(); ++i)
      {
        this->write8 (func, addr++, *i);
      }

    this->write8 (func, addr++, 0);
  }

  /* Write a block of data to memory.  */
  void vm68k_bus::write (function_code func, vm68k_address_t addr,
                         const void *data, size_t size)
  {
    const unsigned char *i = static_cast<const unsigned char *> (data);
    const unsigned char *last = i + size;

    while (i != last)
      {
        this->write8 (func, addr++, *i++);
      }
  }
}
