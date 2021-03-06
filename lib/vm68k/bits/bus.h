/* -*-c++-*-
 * bus - bus unit private header for Virtual M68000 Toolkit
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

#ifndef _VM68K_BUS_H
#define _VM68K_BUS_H 1

#include <exception>
#include <string>
#include <vector>

namespace vx68k
{
  /* Address */
  typedef uint_fast32_t vm68k_address_t;

  const int PAGE_SHIFT = 12;
  const std::size_t PAGE_SIZE = ((std::size_t) 1) << PAGE_SHIFT;

  // External MC68000 address is 24-bit size.
  const int ADDRESS_BIT = 24;
  const std::size_t NPAGES = ((std::size_t) 1) << ADDRESS_BIT - PAGE_SHIFT;

  /**
   * Bus error.
   */
  class VM68K_PUBLIC vm68k_bus_error : public std::exception
  {
  public:
    vm68k_bus_error (uint_fast16_t status, vm68k_address_t addr) throw ();

  public:
    uint_fast16_t status () const throw ()
    {
      return _status;
    }
    vm68k_address_t address () const throw ()
    {
      return _address;
    }
    const char *what () const throw ();

  private:
    uint_least16_t _status;
    vm68k_address_t _address;
  };

  /**
   * Address error.
   */
  class VM68K_PUBLIC vm68k_address_error : public std::exception
  {
  public:
    vm68k_address_error (uint_fast16_t status, vm68k_address_t addr) throw ();

  public:
    uint_fast16_t status () const throw ()
    {
      return _status;
    }
    vm68k_address_t address () const throw ()
    {
      return _address;
    }
    const char *what () const throw ();

  private:
    uint_least16_t _status;
    vm68k_address_t _address;
  };

  /* Maps an address space to memories.  An address space is a
     software view of a target machine.  */
  class VM68K_PUBLIC vm68k_bus
  {
  public:
    /* Function code.  This enum identifies an access type.  */
    enum function_code
    {
      USER_DATA =     1,
      USER_PROGRAM =  2,
      SUPER_DATA =    5,
      SUPER_PROGRAM = 6,
    };

    enum direction_code
    {
      WRITE =    0,
      READ =  0x10,
    };

    /**
     * Object that can be mapped onto a bus.
     */
    class VM68K_PUBLIC mappable
    {
    public:
      virtual ~mappable ();

    public:
      virtual uint_fast8_t read8 (function_code func,
                                  vm68k_address_t addr) const
        throw (vm68k_bus_error);
      virtual uint_fast16_t read16 (function_code func,
                                    vm68k_address_t addr) const
        throw (vm68k_bus_error);
      virtual uint_fast32_t read32 (function_code func,
                                    vm68k_address_t addr) const
        throw (vm68k_bus_error);

      virtual void write8 (function_code func, vm68k_address_t addr,
                           uint_fast8_t value) throw (vm68k_bus_error);
      virtual void write16 (function_code func, vm68k_address_t addr,
                            uint_fast16_t value) throw (vm68k_bus_error);
      virtual void write32 (function_code func, vm68k_address_t addr,
                            uint_fast32_t value) throw (vm68k_bus_error);
    };

  public:
    vm68k_bus ();
    ~vm68k_bus ();

  protected:
    typedef std::vector<mappable *> page_table_type;
  private:
    mappable null_accessible;
    page_table_type page_table[7];

  protected:
    /* Finds a page that contains address ADDR.  */
    page_table_type::iterator find_page (function_code func,
                                         vm68k_address_t addr)
    {
      return page_table[func].begin () + (addr >> PAGE_SHIFT) % NPAGES;
    }

    /* The const version.  */
    page_table_type::const_iterator find_page (function_code func,
                                               vm68k_address_t addr) const
    {
      return (const_cast<vm68k_bus *> (this))->find_page (func, addr);
    }

    /* Fills an address range with memory.  */
    void map_pages (int func_mask, vm68k_address_t addr, uint_fast32_t size,
                    mappable *p);
    void unmap_pages (int func_mask, vm68k_address_t addr, uint_fast32_t size);

  public:
    /* Returns one byte at address ADDR in this address space.  */
    uint_fast8_t read8 (function_code func, vm68k_address_t addr) const
      throw (vm68k_bus_error)
    {
      const mappable *p = *(this->find_page (func, addr));
      return p->read8 (func, addr);
    }

    /* Returns one word at address ADDR in this address space.
       The address must be word-aligned.  */
    uint_fast16_t read16_unchecked (function_code func,
                                    vm68k_address_t addr) const
      throw (vm68k_bus_error)
    {
      const mappable *p = *(this->find_page (func, addr));
      return p->read16 (func, addr);
    }

    /* Returns one word at address ADDR in this address space.  Any
       unaligned address will be handled.  */
    uint_fast16_t read16 (function_code func, vm68k_address_t addr) const
      throw (vm68k_bus_error, vm68k_address_error);

    /* Returns one long word at address ADDR in this address space.
       Any unaligned address will be handled.  */
    uint_fast32_t read32 (function_code func, vm68k_address_t addr) const
      throw (vm68k_bus_error, vm68k_address_error);

    std::string read_string (function_code func, vm68k_address_t addr) const;

    void read (function_code func, vm68k_address_t addr, void *buffer,
               size_t size) const;

    /* Stores byte VALUE at address ADDR in this address space.  */
    void write8 (function_code func, vm68k_address_t addr, uint_fast8_t value)
      throw (vm68k_bus_error)
    {
      mappable *p = *(this->find_page (func, addr));
      p->write8 (func, addr, value);
    }

    /* Stores word VALUE at address ADDR in this address space.
       The address must be word-aligned.  */
    void write16_unchecked (function_code func, vm68k_address_t addr,
                            uint_fast16_t value) throw (vm68k_bus_error)
    {
      mappable *p = *(this->find_page (func, addr));
      p->write16 (func, addr, value);
    }

    /* Stores word VALUE at address ADDR in this address space.
       Any unaligned address will be handled.  */
    void write16 (function_code func, vm68k_address_t addr,
                  uint_fast16_t value)
      throw (vm68k_bus_error, vm68k_address_error);

    /* Stores long word VALUE at address ADDR in this address
       space.  Any unaligned address will be handled.  */
    void write32 (function_code func, vm68k_address_t addr,
                  uint_fast32_t value)
      throw (vm68k_bus_error, vm68k_address_error);

    void write_string (function_code func, vm68k_address_t addr,
                       const std::string &s);

    void write (function_code func, vm68k_address_t addr, const void *buffer,
                size_t size);
  };
}

#endif
