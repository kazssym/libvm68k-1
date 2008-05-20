/* -*-c++-*-
 * addressing - addressing mode for Virtual M68000 Toolkit
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

#ifndef INST_ADDRESSING_H
#define INST_ADDRESSING_H 1

#include <string>
#include <cstdio>
#include <vm68k/context>

#include <cassert>

namespace vx68k_m68k
{
  using namespace vx68k;
  using vx68k::uint_fast16_t;   // avoid ambiguity

  /* Data register direct addressing.  */
  template<class Size>
  class d_reg_direct
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return 0;
    }

    d_reg_direct (int r, vm68k_address_t)
    {
      _regno = vm68k_context::D0 + r;
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->read_reg_unsigned (Size (), _regno);
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->read_reg (Size (), _regno);
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->write_reg (Size (), _regno, value);
    }

    void finish (vm68k_context *) const
    {
    }

    std::string text (const vm68k_context *) const
    {
      char buf[32];
      std::sprintf (buf, "%%d%d", _regno - 0);
      return buf;
    }

  private:
    // XXX: This function is left unimplemented.
    vm68k_address_t address (const vm68k_context *) const;

  private:
    int _regno;
  };

  /* Address register direct addressing.  */
  template<class Size>
  class a_reg_direct
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return 0;
    }

    a_reg_direct (int r, vm68k_address_t)
    {
      _regno = vm68k_context::A0 + r;
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->read_reg_unsigned (Size (), _regno);
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->read_reg (Size (), _regno);
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->write_reg (Size (), _regno, value);
    }

    void finish (vm68k_context *) const
    {
    }

    std::string text (const vm68k_context *) const
    {
      char buf[32];
      std::sprintf (buf, "%%a%d", _regno - 8);
      return buf;
    }

  private:
    // XXX: This function is left unimplemented.
    vm68k_address_t address (const vm68k_context *) const;

  private:
    int _regno;
  };

  template<>
  inline void a_reg_direct<vm68k_word>::
  put (vm68k_context *c, vm68k_word::udata_type value) const
  {
    /* Special case for a word write to an address register.  */
    c->write_reg (vm68k_data_size::LONG_WORD, _regno,
                  vm68k_word::as_signed (value & 0xffffU));
  }

  template<>
  class a_reg_direct<vm68k_byte>
  {
  private:
    // XXX: This function is left unimplemented.
    a_reg_direct ();
  };

  /* Indirect addressing.  */
  template<class Size>
  class indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return 0;
    }

    indirect (int r, vm68k_address_t)
    {
      _regno = vm68k_context::A0 + r;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->read_reg (vm68k_data_size::LONG_WORD, _regno);
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[64];
      std::sprintf (buf, "%%a%d@ /*= %#lx*/", _regno - 8,
                    (unsigned long) this->address (Size (), c));
      return buf;
    }

  private:
    int _regno;
  };

  /* Indirect addressing with postincrement.  */
  template<class Size>
  class postinc_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return 0;
    }

    postinc_indirect (int r, vm68k_address_t)
    {
      _regno = vm68k_context::A0 + r;
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
      c->write_reg (vm68k_data_size::LONG_WORD, _regno,
                    this->address (c) + increment_size ());
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[64];
      std::sprintf (buf, "%%a%d@+ /*= %#lx*/", _regno - 8,
                    (unsigned long) this->address (Size (), c));
      return buf;
    }

  protected:
    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->read_reg (vm68k_data_size::LONG_WORD, _regno);
    }

    uint_fast16_t increment_size () const
    {
      if (_regno == vm68k_context::SP) // XXX: %sp is special.
        {
          return Size::aligned_data_size ();
        }
      else
        {
          return Size::data_size ();
        }
    }

  private:
    int _regno;
  };

  /* Indirect addressing with predecrement.  */
  template<class Size>
  class predec_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return 0;
    }

    predec_indirect (int r, vm68k_address_t)
    {
      _regno = vm68k_context::A0 + r;
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
      c->write_reg (vm68k_data_size::LONG_WORD, _regno, this->address (c));
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[64];
      std::sprintf (buf, "%%a%d@- /*= %#lx*/", _regno - 8,
                    (unsigned long) this->address (Size (), c));
      return buf;
    }

  protected:
    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->read_reg (vm68k_data_size::LONG_WORD, _regno)
        - this->decrement_size ();
    }

    uint_fast16_t decrement_size () const
    {
      if (_regno == vm68k_context::SP) // XXX: %sp is special.
        {
          return Size::aligned_data_size ();
        }
      else
        {
          return Size::data_size ();
        }
    }

  private:
    int _regno;
  };

  /* Indirect addressing with displacement.  */
  template<class Size>
  class disp_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_word::aligned_data_size ();
    }

    disp_indirect (int r, vm68k_address_t pc)
    {
      _regno = vm68k_context::A0 + r;
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->read_reg (vm68k_data_size::LONG_WORD, _regno)
        + c->fetch (vm68k_data_size::WORD, _pc);
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[64];
      std::sprintf (buf, "%%a%d@(%d) /*= %#lx*/", _regno - 8,
                    (int) c->fetch (vm68k_data_size::WORD, _pc),
                    (unsigned long) this->address (c));
      return buf;
    }

  private:
    int _regno;
    vm68k_address_t _pc;
  };

  /* Indirect addressing with index.  */
  template<class Size>
  class index_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_word::aligned_data_size ();
    }

    index_indirect (int r, vm68k_address_t pc)
    {
      _regno = vm68k_context::A0 + r;
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      uint_fast16_t w = c->fetch_unsigned (vm68k_data_size::WORD, _pc);
      int x = w >> 12 & 0xf;
      if ((w & 0x800) != 0)
        {
          return c->read_reg (vm68k_data_size::LONG_WORD, _regno)
            + c->read_reg (vm68k_data_size::LONG_WORD, x)
            + vm68k_byte::as_signed (w & 0xffU);
        }
      else
        {
          return c->read_reg (vm68k_data_size::LONG_WORD, _regno)
            + c->read_reg (vm68k_data_size::WORD, x)
            + vm68k_byte::as_signed (w & 0xffU);
        }
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      uint_fast16_t w = c->fetch_unsigned (vm68k_data_size::WORD, _pc);
      int x = w >> 12 & 0xf;
      const char *xp = x >= 8 ? "%a" : "%d";
      char buf[64];
      if ((w & 0x800) != 0)
        {
          std::sprintf (buf, "%%a%d@(%d,%s%d:l) /*= %#lx*/", _regno - 8,
                        (int) vm68k_byte::as_signed (w & 0xffU), xp, x & 7,
                        (unsigned long) this->address (c));
        }
      else
        {
          std::sprintf (buf, "%%a%d@(%d,%s%d:w) /*= %#lx*/", _regno - 8,
                        (int) vm68k_byte::as_signed (w & 0xffU), xp, x & 7,
                        (unsigned long) this->address (c));
        }
      return buf;
    }

  private:
    int _regno;
    vm68k_address_t _pc;
  };

  /* Absolute short addressing.  */
  template<class Size>
  class abs_short
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_word::aligned_data_size ();
    }

    abs_short (int, vm68k_address_t pc)
    {
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->fetch (vm68k_data_size::WORD, _pc);
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[32];
      std::sprintf (buf, "%#lx:w", (unsigned long) this->address (c));
      return buf;
    }

  private:
    vm68k_address_t _pc;
  };

  /* Absolute long addressing.  */
  template<class Size>
  class abs_long
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_long_word::aligned_data_size ();
    }

    abs_long (int, vm68k_address_t pc)
    {
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      return c->fetch (vm68k_data_size::LONG_WORD, _pc);
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void put (vm68k_context *c, typename Size::udata_type value) const
    {
      c->store (Size (), this->address (c), value);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[32];
      std::sprintf (buf, "%#lx", (unsigned long) this->address (c));
      return buf;
    }

  private:
    vm68k_address_t _pc;
  };

  /* PC indirect addressing with displacement.  */
  template<class Size>
  class disp_pc_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_word::aligned_data_size ();
    }

    disp_pc_indirect (int, vm68k_address_t pc)
    {
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      return _pc + c->fetch (vm68k_data_size::WORD, _pc);
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[64];
      std::sprintf (buf, "%%pc@(%d) /*= %#lx*/",
                    (int) c->fetch (vm68k_data_size::WORD, _pc),
                    (unsigned long) this->address (c));
      return buf;
    }

  private:
    // XXX: This function is left unimplemented.
    void put (vm68k_context *, typename Size::udata_type) const;

  private:
    vm68k_address_t _pc;
  };

  /* PC indirect addressing with index.  */
  template<class Size>
  class index_pc_indirect
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return vm68k_word::aligned_data_size ();
    }

    index_pc_indirect (int, vm68k_address_t pc)
    {
      _pc = pc;
    }

    vm68k_address_t address (const vm68k_context *c) const
    {
      uint_fast16_t w = c->fetch_unsigned (vm68k_data_size::WORD, _pc);
      int x = w >> 12 & 0xf;
      if ((w & 0x800) != 0)
        {
          return _pc + c->read_reg (vm68k_data_size::LONG_WORD, x)
            + vm68k_byte::as_signed (w & 0xffU);
        }
      else
        {
          return _pc + c->read_reg (vm68k_data_size::WORD, x)
            + vm68k_byte::as_signed (w & 0xffU);
        }
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->load_unsigned (Size (), this->address (c));
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->load (Size (), this->address (c));
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      uint_fast16_t w = c->fetch_unsigned (vm68k_data_size::WORD, _pc);
      int x = w >> 12 & 0xf;
      const char *xp = x >= 8 ? "%a" : "%d";
      char buf[64];
      if ((w & 0x800) != 0)
        {
          std::sprintf (buf, "%%pc@(%d,%s%d:l) /*= %#lx*/",
                        (int) vm68k_byte::as_signed (w & 0xffU), xp, x & 7,
                        (unsigned long) this->address (c));
        }
      else
        {
          std::sprintf (buf, "%%pc@(%d,%s%d:w) /*= %#lx*/",
                        (int) vm68k_byte::as_signed (w & 0xffU), xp, x & 7,
                        (unsigned long) this->address (c));
        }
      return buf;
    }

  private:
    // XXX: This function is left unimplemented.
    void put (vm68k_context *, typename Size::udata_type) const;

  private:
    vm68k_address_t _pc;
  };

  /* Immediate addressing.  */
  template<class Size>
  class immediate
  {
  public:
    static uint_fast16_t extension_size ()
    {
      return Size::aligned_data_size ();
    }

    immediate (int, vm68k_address_t pc)
    {
      _pc = pc;
    }

    typename Size::udata_type get_unsigned (const vm68k_context *c) const
    {
      return c->fetch_unsigned (Size (), _pc);
    }

    typename Size::data_type get (const vm68k_context *c) const
    {
      return c->fetch (Size (), _pc);
    }

    void finish (vm68k_context *c) const
    {
    }

    std::string text (const vm68k_context *c) const
    {
      char buf[32];
      std::sprintf (buf, "#%#lx", (unsigned long) this->get_unsigned (c));
      return buf;
    }

  private:
    // XXX: These functions are left unimplemented.
    vm68k_address_t address (const vm68k_context *) const;
    void put (vm68k_context *, typename Size::udata_type) const;

  private:
    vm68k_address_t _pc;
  };
}

#endif
