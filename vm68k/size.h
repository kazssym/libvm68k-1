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

#ifndef __VM68K_SIZE_H
#define __VM68K_SIZE_H 1

#include <vm68k/bus>
#include <vm68k/types.h>

namespace vm68k
{
  using namespace std;

  /* Access methods for byte data.  */
  struct byte_size
  {
    typedef int uvalue_type;
    typedef int value_type;
    typedef int svalue_type;

    static size_t value_bit() {return 8;}
    static int value_mask() {return (1 << value_bit()) - 1;}
    static size_t value_size() {return 1;}
    static size_t aligned_value_size() {return 2;}

    static int uvalue(int value);
    static int svalue(int value);

    static int uget(const uint32_type &reg);
    static int uget(const bus &, function_code, uint32_type address);
    static int get(const uint32_type &reg);
    static int get(const bus &, function_code, uint32_type address);

    static void put(uint32_type &reg, int value);
    static void put(bus &, function_code, uint32_type address, int value);

    static const char *suffix() {return "b";}
  };

  /* Access methods for word data.  */
  struct word_size
  {
    typedef uint16_type uvalue_type;
    typedef sint16_type value_type;
    typedef sint16_type svalue_type;

    static size_t value_bit() {return 16;}
    static uint16_type value_mask()
    {return (uint16_type(1) << value_bit()) - 1;}
    static size_t value_size() {return 2;}
    static size_t aligned_value_size() {return value_size();}

    static uint16_type uvalue(uint16_type value);
    static sint16_type svalue(uint16_type value);

    static uint16_type uget(const uint32_type &reg);
    static uint16_type uget_aligned(const bus &, function_code,
				      uint32_type address);
    static uint16_type uget(const bus &, function_code, uint32_type address);
    static sint16_type get(const uint32_type &reg);
    static sint16_type get_aligned(const bus &, function_code,
				     uint32_type address);
    static sint16_type get(const bus &, function_code, uint32_type address);

    static void put(uint32_type &reg, uint16_type value);
    static void put(bus &, function_code,
		    uint32_type address, uint16_type value);

    static const char *suffix() {return "w";}
  };

  /* Access methods for long word data.  */
  struct long_word_size
  {
    typedef uint32_type uvalue_type;
    typedef sint32_type value_type;
    typedef sint32_type svalue_type;

    static size_t value_bit() {return 32;}
    static uint32_type value_mask()
      //{return (uint32_type(1) << value_bit()) - 1;}
      {return 0xffffffffu;}
    static size_t value_size() {return 4;}
    static size_t aligned_value_size() {return value_size();}

    static uint32_type uvalue(uint32_type value);
    static sint32_type svalue(uint32_type value);

    static uint32_type uget(const uint32_type &reg);
    static uint32_type uget_aligned(const bus &, function_code,
				      uint32_type address);
    static uint32_type uget(const bus &,
			    function_code, uint32_type address);
    static sint32_type get(const uint32_type &reg);
    static sint32_type get_aligned(const bus &,
				     function_code,
				     uint32_type address);
    static sint32_type get(const bus &,
			   function_code, uint32_type address);

    static void put(uint32_type &reg, uint32_type value);
    static void put(bus &, function_code,
		    uint32_type address, uint32_type value);

    static const char *suffix() {return "l";}
  };
  
  inline int
  byte_size::uvalue(int value)
  {
    return value & value_mask();
  }

  inline int
  byte_size::svalue(int value)
  {
    value = uvalue(value);
    const int N = 1 << value_bit() - 1;
    if (value >= N)
      return -int(value_mask() - value) - 1;
    else
      return value;
  }

  inline int
  byte_size::uget(const uint32_type &reg)
  {
    return uvalue(reg);
  }

  inline int
  byte_size::uget(const bus &m,
		  function_code fc, uint32_type address)
  {
    return m.get_8(address, fc);
  }

  inline int
  byte_size::get(const uint32_type &reg)
  {
    return svalue(uget(reg));
  }

  inline int
  byte_size::get(const bus &m, function_code fc,
		 uint32_type address)
  {
    return svalue(uget(m, fc, address));
  }

  inline void
  byte_size::put(uint32_type &reg, int value)
  {
    reg = reg & ~uint32_type(value_mask()) | uvalue(value);
  }

  inline void
  byte_size::put(bus &m, function_code fc,
		 uint32_type address, int value)
  {
    m.put_8(address, value, fc);
  }
  
  inline uint16_type
  word_size::uvalue(uint16_type value)
  {
    return value & value_mask();
  }

  inline sint16_type
  word_size::svalue(uint16_type value)
  {
    value = uvalue(value);
    const uint16_type N = uint16_type(1) << value_bit() - 1;
    if (value >= N)
      return -sint16_type(value_mask() - value) - 1;
    else
      return value;
  }

  inline uint16_type
  word_size::uget(const uint32_type &reg)
  {
    return uvalue(reg);
  }

  inline uint16_type
  word_size::uget_aligned(const bus &m,
			    function_code fc, uint32_type address)
  {
    return m.get_16_aligned(address, fc);
  }

  inline uint16_type
  word_size::uget(const bus &m,
		  function_code fc, uint32_type address)
  {
    return m.get_16(address, fc);
  }

  inline sint16_type
  word_size::get(const uint32_type &reg)
  {
    return svalue(uget(reg));
  }

  inline sint16_type
  word_size::get_aligned(const bus &m,
			   function_code fc, uint32_type address)
  {
    return svalue(uget_aligned(m, fc, address));
  }

  inline sint16_type
  word_size::get(const bus &m,
		 function_code fc, uint32_type address)
  {
    return svalue(uget(m, fc, address));
  }

  inline void
  word_size::put(uint32_type &reg, uint16_type value)
  {
    reg = reg & ~uint32_type(value_mask()) | uvalue(value);
  }

  inline void
  word_size::put(bus &m, function_code fc,
		 uint32_type address, uint16_type value)
  {
    m.put_16(address, value, fc);
  }
  
  inline uint32_type
  long_word_size::uvalue(uint32_type value)
  {
    return value & value_mask();
  }

  inline sint32_type
  long_word_size::svalue(uint32_type value)
  {
    value &= value_mask();
    const uint32_type N = uint32_type(1) << value_bit() - 1;
    if (value >= N)
      return -sint32_type(value_mask() - value) - 1;
    else
      return value;
  }

  inline uint32_type
  long_word_size::uget(const uint32_type &reg)
  {
    return uvalue(reg);
  }

  inline uint32_type
  long_word_size::uget_aligned(const bus &m,
				 function_code fc, uint32_type address)
  {
    return m.get_32(address, fc);
  }

  inline uint32_type
  long_word_size::uget(const bus &m,
		       function_code fc, uint32_type address)
  {
    return m.get_32(address, fc);
  }

  inline sint32_type
  long_word_size::get(const uint32_type &reg)
  {
    return svalue(uget(reg));
  }

  inline sint32_type
  long_word_size::get_aligned(const bus &m,
				function_code fc, uint32_type address)
  {
    return svalue(uget_aligned(m, fc, address));
  }

  inline sint32_type
  long_word_size::get(const bus &m,
		      function_code fc, uint32_type address)
  {
    return svalue(uget(m, fc, address));
  }

  inline void
  long_word_size::put(uint32_type &reg, uint32_type value)
  {
    reg = reg & ~uint32_type(value_mask()) | uvalue(value);
  }

  inline void
  long_word_size::put(bus &m, function_code fc,
		      uint32_type address, uint32_type value)
  {
    m.put_32(address, value, fc);
  }
}

#endif /* not __VM68K_SIZE_H */
