/* -*-C++-*- */
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

#ifndef __VM68K_MEMORY_H
#define __VM68K_MEMORY_H 1

#include <vm68k/types.h>
#include <vector>
#include <string>
#include <exception>

namespace vm68k
{
  using namespace std;

  const int PAGE_SHIFT = 12;
  const uint32_type PAGE_SIZE = uint32_type(1) << PAGE_SHIFT;

  // External mc68000 address is 24-bit size.
  const int ADDRESS_BIT = 24;
  const uint32_type NPAGES = uint32_type(1) << ADDRESS_BIT - PAGE_SHIFT;

  /* Bus error or address error.  */
  class memory_exception: public exception
  {
  public:
    uint32_type _address;
    uint16_type _status;

  protected:
    memory_exception(uint32_type address, uint16_type status) throw ();

  public:
    virtual int vecno() throw () = 0;
  };

  /* Bus error exception.  */
  struct bus_error: memory_exception
  {
    bus_error(uint32_type address, uint16_type status) throw ();
    int vecno() throw ();
    const char *what() const throw ();
  };

  /* Address error exception.  */
  struct address_error: memory_exception
  {
    address_error(uint32_type address, uint16_type status) throw ();
    int vecno() throw ();
    const char *what() const throw ();
  };

  /* Abstract memory class.  */
  class memory
  {
  public:
    /* Function code, which identifies an access type.  */
    enum function_code
    {
      USER_DATA = 1,
      USER_PROGRAM = 2,
      SUPER_DATA = 5,
      SUPER_PROGRAM = 6
    };
    enum access
    {
      WRITE = 0,
      READ = 0x10
    };

  public:
    virtual ~memory() {}

  public:
    virtual int get_8(uint32_type address, function_code fc) const
      throw (memory_exception) = 0;
    virtual uint16_type get_16(uint32_type address, function_code fc) const
      throw (memory_exception) = 0;
    virtual uint32_type get_32(uint32_type address, function_code fc) const
      throw (memory_exception);

    virtual void put_8(uint32_type address, int value, function_code fc)
      throw (memory_exception) = 0;
    virtual void put_16(uint32_type address, uint16_type value,
			function_code fc)
      throw (memory_exception) = 0;
    virtual void put_32(uint32_type address, uint32_type value,
			function_code fc)
      throw (memory_exception);
  };

  /* Maps an address space to memories.  An address space is a
     software view of a target machine.  */
  class memory_map
  {
  public:
    typedef memory::function_code function_code;

  private:
    vector<memory *> page_table;

  public:
    memory_map();
    virtual ~memory_map();

  public:
    static memory *null_memory() throw ();

  protected:
    /* Finds a page that contains address ADDRESS.  */
    vector<memory *>::const_iterator find_memory(uint32_type address) const;
    vector<memory *>::iterator find_memory(uint32_type address);

  public:
    /* Fills an address range with memory.  */
    void fill(uint32_type, uint32_type, memory *);

  public:
    /* Returns one byte at address ADDRESS in this address space.  */
    int get_8(uint32_type address, function_code fc) const
      throw (memory_exception);

    /* Returns one word at address ADDRESS in this address space.  Any
       unaligned address will be handled.  */
    uint16_type get_16(uint32_type address, function_code fc) const
      throw (memory_exception);

    /* Returns one word at address ADDRESS in this address space.
       The address must be word-aligned.  */
    uint16_type get_16_unchecked(uint32_type address, function_code fc) const
      throw (memory_exception);

    /* Returns one long word at address ADDRESS in this address space.
       Any unaligned address will be handled.  */
    uint32_type get_32(uint32_type address, function_code fc) const
      throw (memory_exception);

    string get_string(uint32_type address, function_code fc) const;

    void read(uint32_type, void *, size_t, function_code fc) const;

    /* Stores byte VALUE at address ADDRESS in this address space.  */
    void put_8(uint32_type address, int value, function_code fc)
      throw (memory_exception);

    /* Stores word VALUE at address ADDRESS in this address space.
       Any unaligned address will be handled.  */
    void put_16(uint32_type address, uint16_type value, function_code fc)
      throw (memory_exception);

    /* Stores word VALUE at address ADDRESS in this address space.
       The address must be word-aligned.  */
    void put_16_unchecked(uint32_type address, uint16_type value,
			  function_code fc)
      throw (memory_exception);

    /* Stores long word VALUE at address ADDRESS in this address
       space.  Any unaligned address will be handled.  */
    void put_32(uint32_type address, uint32_type value, function_code fc)
      throw (memory_exception);

    void put_string(uint32_type address, const string &, function_code fc);

    void write(uint32_type, const void *, size_t, function_code fc);
  };
  
  inline
  memory_exception::memory_exception(uint32_type address,
				     uint16_type status) throw ()
    : _address(address), _status(status)
  {
  }

  inline
  bus_error::bus_error(uint32_type address,
		       uint16_type status) throw ()
    : memory_exception(address, status)
  {
  }

  inline
  address_error::address_error(uint32_type address,
			       uint16_type status) throw ()
    : memory_exception(address, status)
  {
  }

  inline vector<memory *>::const_iterator
  memory_map::find_memory(uint32_type address) const
  {
    return page_table.begin() + (address >> PAGE_SHIFT) % NPAGES;
  }

  inline vector<memory *>::iterator
  memory_map::find_memory(uint32_type address)
  {
    return page_table.begin() + (address >> PAGE_SHIFT) % NPAGES;
  }

  inline int
  memory_map::get_8(uint32_type address, function_code fc) const
    throw (memory_exception)
  {
    const memory *p = *this->find_memory(address);
    return p->get_8(address, fc);
  }

  inline uint16_type
  memory_map::get_16_unchecked(uint32_type address, function_code fc) const
    throw (memory_exception)
  {
    const memory *p = *this->find_memory(address);
    return p->get_16(address, fc);
  }

  inline void
  memory_map::put_8(uint32_type address, int value, function_code fc)
    throw (memory_exception)
  {
    memory *p = *this->find_memory(address);
    p->put_8(address, value, fc);
  }

  inline void
  memory_map::put_16_unchecked(uint32_type address, uint16_type value,
			       function_code fc)
    throw (memory_exception)
  {
    memory *p = *this->find_memory(address);
    p->put_16(address, value, fc);
  }
}

#endif /* not __VM68K_MEMORY_H */
