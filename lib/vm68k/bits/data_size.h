/* -*-c++-*- */
/* data_size - data_size unit private header for Virtual M68000 Toolkit
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

#ifndef _VM68K_DATA_SIZE_H
#define _VM68K_DATA_SIZE_H 1

namespace vx68k
{
  /* Forward declarations.  */
  struct vm68k_byte_size;
  struct vm68k_word_size;
  struct vm68k_long_word_size;

  /* Access methods for byte data.  */
  struct VM68K_PUBLIC vm68k_byte_size
  {
    typedef vm68k_int_fast8_t  data_type;
    typedef vm68k_uint_fast8_t udata_type;

    typedef void half_size;

    static vm68k_uint_fast16_t data_size ()
    {
      return 1;
    }

    static vm68k_uint_fast16_t aligned_data_size ()
    {
      return 2;
    }

    static vm68k_int_fast8_t as_signed (vm68k_uint_fast8_t value)
    {
      if (value > 0x7fU)
        return -((vm68k_int_fast8_t) (0xffU - value)) - 1;
      else
        return (vm68k_int_fast8_t) value;
    }

    static vm68k_uint_fast8_t read_unsigned (const vm68k_uint32_t &reg)
    {
      return reg & 0xffU;
    }

    static vm68k_int_fast8_t read (const vm68k_uint32_t &reg)
    {
      return as_signed (reg & 0xffU);
    }

    static void write (vm68k_uint32_t &reg, vm68k_uint_fast8_t value)
    {
      reg = reg & ~0xffU | value & 0xffU;
    }

    static vm68k_uint_fast8_t read_unsigned (const vm68k_bus *bus,
                                             vm68k_bus_function func,
                                             vm68k_address_t addr)
    {
      return bus->read8 (func, addr);
    }

    static vm68k_int_fast8_t read (const vm68k_bus *bus,
                                   vm68k_bus_function func,
                                   vm68k_address_t addr)
    {
      return as_signed (bus->read8 (func, addr));
    }

    static void write (vm68k_bus *bus,
                       vm68k_bus_function func,
                       vm68k_address_t addr, vm68k_uint_fast8_t value)
    {
      bus->write8 (func, addr, value);
    }

    static vm68k_uint_fast8_t read_inst_unsigned (const vm68k_bus *bus,
                                                  vm68k_bus_function func,
                                                  vm68k_address_t addr)
    {
      return bus->read8 (func, addr | 1U);
    }

    static vm68k_int_fast8_t read_inst (const vm68k_bus *bus,
                                        vm68k_bus_function func,
                                        vm68k_address_t addr)
    {
      return as_signed (bus->read8 (func, addr | 1U));
    }

    static const char *suffix ()
    {
      return "b";
    }
  };

  /* Access methods for word data.  */
  struct VM68K_PUBLIC vm68k_word_size
  {
    typedef vm68k_int_fast16_t  data_type;
    typedef vm68k_uint_fast16_t udata_type;

    typedef vm68k_byte_size half_size;

    static vm68k_uint_fast16_t data_size ()
    {
      return 2;
    }

    static vm68k_uint_fast16_t aligned_data_size ()
    {
      return 2;
    }

    static vm68k_int_fast16_t as_signed (vm68k_uint_fast16_t value)
    {
      if (value > 0x7fffU)
        return -((vm68k_int_fast16_t) (0xffffU - value)) - 1;
      else
        return (vm68k_int_fast16_t) value;
    }

    static vm68k_uint_fast16_t read_unsigned (const vm68k_uint32_t &reg)
    {
      return reg & 0xffffU;
    }

    static vm68k_int_fast16_t read (const vm68k_uint32_t &reg)
    {
      return as_signed (reg & 0xffffU);
    }

    static void write (vm68k_uint32_t &reg, vm68k_uint_fast16_t value)
    {
      reg = reg & ~0xffffU | value & 0xffffU;
    }

    static vm68k_uint_fast16_t read_unsigned (const vm68k_bus *bus,
                                              vm68k_bus_function func,
                                              vm68k_address_t addr)
    {
      return bus->read16 (func, addr);
    }

    static vm68k_int_fast16_t read (const vm68k_bus *bus,
                                    vm68k_bus_function func,
                                    vm68k_address_t addr)
    {
      return as_signed (bus->read16 (func, addr));
    }

    static void write (vm68k_bus *bus,
                       vm68k_bus_function func,
                       vm68k_address_t addr, vm68k_uint_fast16_t value)
    {
      bus->write16 (func, addr, value);
    }

    static vm68k_uint_fast16_t read_inst_unsigned (const vm68k_bus *bus,
                                                   vm68k_bus_function func,
                                                   vm68k_address_t addr)
    {
      return bus->read16 (func, addr);
    }

    static vm68k_int_fast16_t read_inst (const vm68k_bus *bus,
                                         vm68k_bus_function func,
                                         vm68k_address_t addr)
    {
      return as_signed (bus->read16 (func, addr));
    }

    static const char *suffix ()
    {
      return "w";
    }
  };

  /* Access methods for long word data.  */
  struct VM68K_PUBLIC vm68k_long_word_size
  {
    typedef vm68k_int_fast32_t  data_type;
    typedef vm68k_uint_fast32_t udata_type;

    typedef vm68k_word_size half_size;

    static vm68k_uint_fast16_t data_size ()
    {
      return 4;
    }

    static vm68k_uint_fast32_t aligned_data_size ()
    {
      return 4;
    }

    static vm68k_int_fast32_t as_signed (vm68k_uint_fast32_t value)
    {
      if (value > 0x7fffffffUL)
        return -((vm68k_int_fast32_t) (0xffffffffUL - value)) - 1;
      else
        return (vm68k_int_fast32_t) value;
    }

    static vm68k_uint_fast32_t read_unsigned (const vm68k_uint32_t &reg)
    {
      return reg;
    }

    static vm68k_int_fast32_t read (const vm68k_uint32_t &reg)
    {
      return as_signed (reg);
    }

    static void write (vm68k_uint32_t &reg, vm68k_uint_fast32_t value)
    {
      reg = value & 0xffffffffUL;
    }

    static vm68k_uint_fast32_t read_unsigned (const vm68k_bus *bus,
                                              vm68k_bus_function func,
                                              vm68k_address_t addr)
    {
      return bus->read32 (func, addr);
    }

    static vm68k_int_fast32_t read (const vm68k_bus *bus,
                                    vm68k_bus_function func,
                                    vm68k_address_t addr)
    {
      return as_signed (bus->read32 (func, addr));
    }

    static void write (vm68k_bus *bus,
                       vm68k_bus_function func,
                       vm68k_address_t addr, vm68k_uint_fast32_t value)
    {
      bus->write32 (func, addr, value);
    }

    static vm68k_uint_fast32_t read_inst_unsigned (const vm68k_bus *bus,
                                                   vm68k_bus_function func,
                                                   vm68k_address_t addr)
    {
      return bus->read32 (func, addr);
    }

    static vm68k_int_fast32_t read_inst (const vm68k_bus *bus,
                                         vm68k_bus_function func,
                                         vm68k_address_t addr)
    {
      return as_signed (bus->read32 (func, addr));
    }

    static const char *suffix ()
    {
      return "l";
    }
  };

  extern VM68K_PUBLIC const vm68k_byte_size      vm68k_byte;
  extern VM68K_PUBLIC const vm68k_word_size      vm68k_word;
  extern VM68K_PUBLIC const vm68k_long_word_size vm68k_long_word;
}

#endif
