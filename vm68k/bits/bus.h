/* -*- C++ -*- */
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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.  */

#ifndef _VM68K_BUS
#define _VM68K_BUS 1

#include <exception>
#include <string>
#include <vector>
#include <vm68k/base>

namespace vx68k_m68k
{
  const int PAGE_SHIFT = 12;
  const std::size_t PAGE_SIZE = ((std::size_t) 1) << PAGE_SHIFT;

  // External MC68000 address is 24-bit size.
  const int ADDRESS_BIT = 24;
  const std::size_t NPAGES = ((std::size_t) 1) << ADDRESS_BIT - PAGE_SHIFT;

  /* Function code.  This enum identifies an access type.  */
  enum function_code
  {
    USER_DATA =     1,
    USER_PROGRAM =  2,
    SUPER_DATA =    5,
    SUPER_PROGRAM = 6
  };

  enum bus_direction
  {
    WRITE = 0,
    READ =  0x10
  };

  /**
   * Bus error.
   */
  class _VM68K_PUBLIC bus_error : public std::exception
  {
  public:
    bus_error (udata_fast16_t status, address_t address) throw ();

  public:
    udata_fast16_t ssw () const throw () { return _ssw; }
    address_t address () const throw () { return _address; }
    const char *what () const throw ();

  private:
    udata16_t _ssw;
    address_t _address;
  };

  /**
   * Address error.
   */
  class _VM68K_PUBLIC address_error : public std::exception
  {
  public:
    address_error (udata_fast16_t status, address_t address) throw ();

  public:
    udata_fast16_t ssw () const throw () { return _ssw; }
    address_t address () const throw () { return _address; }
    const char *what () const throw ();

  private:
    udata16_t _ssw;
    address_t _address;
  };

  /* Abstract bus target.  A bus target will be mapped within an address
     space.  */
  class _VM68K_PUBLIC accessible
  {
  public:
    virtual ~accessible () {}

  public:
    virtual udata_fast8_t read8 (function_code fc, address_t address) const
      throw (bus_error);
    virtual udata_fast16_t read16 (function_code fc, address_t address) const
      throw (bus_error);
    virtual udata_fast32_t read32 (function_code fc, address_t address) const
      throw (bus_error);

    virtual void write8 (function_code fc, address_t address,
                         udata_fast8_t value)
      throw (bus_error);
    virtual void write16 (function_code fc, address_t address,
                          udata_fast16_t value)
      throw (bus_error);
    virtual void write32 (function_code fc, address_t address,
                        udata_fast32_t value)
      throw (bus_error);
  };

  /* Maps an address space to memories.  An address space is a
     software view of a target machine.  */
  class _VM68K_PUBLIC system_bus
  {
  public:
    system_bus ();
    virtual ~system_bus ();

  protected:
    typedef std::vector<accessible *> page_table_type;
  private:
    page_table_type page_table[7];

  protected:
    /* Finds a page that contains address ADDRESS.  */
    page_table_type::iterator find_page (function_code fc, address_t address)
    {
      return page_table[fc].begin () + (address >> PAGE_SHIFT) % NPAGES;
    }

    /* The const version.  */
    page_table_type::const_iterator find_page (function_code fc,
                                               address_t address) const
    {
      return (const_cast<system_bus *> (this))->find_page (fc, address);
    }

    /* Fills an address range with memory.  */
    void map_page (int bits, address_t address, udata_fast32_t size,
                   accessible *p);
    void unmap_page (int bits, address_t address, udata_fast32_t size);

  public:
    /* Returns one byte at address ADDRESS in this address space.  */
    udata_fast8_t read8 (function_code fc, address_t address) const
      throw (bus_error)
    {
      const accessible *p = *(this->find_page (fc, address));
      return p->read8 (fc, address);
    }

    /* Returns one word at address ADDRESS in this address space.
       The address must be word-aligned.  */
    udata_fast16_t read16_unchecked (function_code fc, address_t address) const
      throw (bus_error)
    {
      const accessible *p = *(this->find_page (fc, address));
      return p->read16 (fc, address);
    }

    /* Returns one word at address ADDRESS in this address space.  Any
       unaligned address will be handled.  */
    udata_fast16_t read16 (function_code fc, address_t address) const
      throw (bus_error, address_error);

    /* Returns one long word at address ADDRESS in this address space.
       Any unaligned address will be handled.  */
    udata_fast32_t read32 (function_code fc, address_t address) const
      throw (bus_error, address_error);

    std::string read_string (function_code fc, address_t address) const;

    void read (function_code fc, address_t address,
               void *buffer, size_t size) const;

    /* Stores byte VALUE at address ADDRESS in this address space.  */
    void write8 (function_code fc, address_t address, udata_fast8_t value)
      throw (bus_error)
    {
      accessible *p = *(this->find_page (fc, address));
      p->write8 (fc, address, value);
    }

    /* Stores word VALUE at address ADDRESS in this address space.
       The address must be word-aligned.  */
    void write16_unchecked (function_code fc, address_t address,
                            udata_fast16_t value)
      throw (bus_error)
    {
      accessible *p = *(this->find_page (fc, address));
      p->write16 (fc, address, value);
    }

    /* Stores word VALUE at address ADDRESS in this address space.
       Any unaligned address will be handled.  */
    void write16 (function_code fc, address_t address, udata_fast16_t value)
      throw (bus_error, address_error);

    /* Stores long word VALUE at address ADDRESS in this address
       space.  Any unaligned address will be handled.  */
    void write32 (function_code fc, address_t address, udata_fast32_t value)
      throw (bus_error, address_error);

    void write_string (function_code fc, address_t address,
                       const std::string &s);

    void write (function_code fc, address_t address,
                const void *buffer, size_t size);
  };
}

#endif
