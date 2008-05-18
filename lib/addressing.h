/* -*-C++-*- */
/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2002 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _VM68K_ADDRESSING
#define _VM68K_ADDRESSING 1

#include <vm68k/processor>
#include <vm68k/size>
#include <string>
#include <cstdio>

namespace vm68k
{
  namespace addressing
  {
    /* Data register direct addressing.  */
    class data_register_direct
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return 0;}

      data_register_direct(int reg, uint32_type address);

      // XXX: address is left unimplemented.

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;
    };

    inline
    data_register_direct::data_register_direct(int reg, uint32_type address)
      : _reg(reg)
    {
    }

    template <class Size>
    inline typename Size::uvalue_type
    data_register_direct::get_u(Size, const context &c) const
    {
      return Size::get_u(c.regs.d[_reg]);
    }

    template <class Size>
    inline typename Size::svalue_type
    data_register_direct::get_s(Size, const context &c) const
    {
      return Size::get_s(c.regs.d[_reg]);
    }

    template <class Size>
    inline void
    data_register_direct::put(Size, context &c,
			      typename Size::uvalue_type value) const
    {
      Size::put(c.regs.d[_reg], value);
    }

    template <class Size>
    inline std::string
    data_register_direct::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%d%d", _reg);
      return buf;
    }

    /* Address register direct addressing.  */
    class address_register_direct
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return 0;}

      address_register_direct(int reg, uint32_type address);

      // XXX: address is left unimplemented.

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;
    };

    inline
    address_register_direct::address_register_direct(int reg,
						     uint32_type address)
      : _reg(reg)
    {
    }

    template <class Size>
    inline typename Size::uvalue_type
    address_register_direct::get_u(Size, const context &c) const
    {
      return Size::get_u(c.regs.a[_reg]);
    }

    template <class Size>
    inline typename Size::svalue_type
    address_register_direct::get_s(Size, const context &c) const
    {
      return Size::get_s(c.regs.a[_reg]);
    }

    template <class Size>
    inline void
    address_register_direct::put(Size, context &c,
				 typename Size::uvalue_type value) const
    {
      c.regs.a[_reg] = long_word::normal_u(value);
    }

    template <class Size>
    inline std::string
    address_register_direct::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%a%d", _reg);
      return buf;
    }

    /* Indirect addressing.  */
    class indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return 0;}

      indirect(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;
    };

    inline
    indirect::indirect(int reg, uint32_type address)
      : _reg(reg)
    {
    }

    template <class Size>
    inline uint32_type
    indirect::address(Size, const context &c) const
    {
      return c.regs.a[_reg];
    }

    template <class Size>
    inline typename Size::uvalue_type
    indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline void
    indirect::put(Size, context &c, typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, address(Size(), c), c.data_fc(), value);
    }

    template <class Size>
    inline std::string
    indirect::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%a%d@/*%#lx*/", _reg, address(Size(), c) + 0UL);
      return buf;
    }

    /* Indirect addressing with postincrement.  */
    class postinc_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return 0;}

      postinc_indirect(int reg, uint32_type address);

      // XXX: address is left unimplemented.

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const;

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;

      template <class Size>
      uint32_type increment_size(Size) const;
    };

    inline
    postinc_indirect::postinc_indirect(int reg, uint32_type address)
      : _reg(reg)
    {
    }

    template <class Size>
    inline uint32_type
    postinc_indirect::increment_size(Size) const
    {
      if (_reg == 7)		// XXX: %a7 is special.
	return Size::aligned_size();
      else
	return Size::size();
    }

    template <class Size>
    inline typename Size::uvalue_type
    postinc_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, c.regs.a[_reg], c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    postinc_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, c.regs.a[_reg], c.data_fc());
    }

    template <class Size>
    inline void
    postinc_indirect::put(Size, context &c,
			  typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, c.regs.a[_reg], c.data_fc(), value);
    }

    template <class Size>
    inline void
    postinc_indirect::finish(Size, context &c) const
    {
      c.regs.a[_reg]
	= long_word::normal_u(c.regs.a[_reg] + increment_size(Size()));
    }

    template <class Size>
    inline std::string
    postinc_indirect::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%a%d@+/*%#lx*/", _reg, c.regs.a[_reg] + 0UL);
      return buf;
    }

    /* Indirect addressing with predecrement.  */
    class predec_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return 0;}

      predec_indirect(int reg, uint32_type address);

      // XXX: address is left unimplemented.

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const;

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;

      template <class Size>
      uint32_type decrement_size(Size) const;
    };

    inline
    predec_indirect::predec_indirect(int reg, uint32_type address)
      : _reg(reg)
    {
    }

    template <class Size>
    inline uint32_type
    predec_indirect::decrement_size(Size) const
    {
      if (_reg == 7)		// XXX: %a7 is special.
	return Size::aligned_size();
      else
	return Size::size();
    }

    template <class Size>
    inline typename Size::uvalue_type
    predec_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, c.regs.a[_reg] - decrement_size(Size()),
			 c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    predec_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, c.regs.a[_reg] - decrement_size(Size()),
			 c.data_fc());
    }

    template <class Size>
    inline void
    predec_indirect::put(Size, context &c,
			  typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, c.regs.a[_reg] - decrement_size(Size()), c.data_fc(),
		value);
    }

    template <class Size>
    inline void
    predec_indirect::finish(Size, context &c) const
    {
      c.regs.a[_reg]
	= long_word::normal_u(c.regs.a[_reg] - decrement_size(Size()));
    }

    template <class Size>
    inline std::string
    predec_indirect::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%a%d@-/*%#lx*/", _reg,
		   long_word::normal_u(c.regs.a[_reg] - decrement_size(Size()))
		   + 0UL);
      return buf;
    }

    /* Indirect addressing with displacement.  */
    class disp_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return word::size();}

      disp_indirect(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;
      uint32_type _address;
    };

    inline
    disp_indirect::disp_indirect(int reg, uint32_type address)
      : _reg(reg),
	_address(address)
    {
    }

    template <class Size>
    inline uint32_type
    disp_indirect::address(Size, const context &c) const
    {
      return long_word::normal_u(c.regs.a[_reg] + c.fetch_s(word(), _address));
    }

    template <class Size>
    inline typename Size::uvalue_type
    disp_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    disp_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline void
    disp_indirect::put(Size, context &c,
		       typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, address(Size(), c), c.data_fc(), value);
    }

    template <class Size>
    inline std::string
    disp_indirect::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%a%d@(%d)/*%#lx*/", _reg,
		   c.fetch_s(word(), _address), address(Size(), c) + 0UL);
      return buf;
    }

    /* Indirect addressing with index.  */
    class index_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return word::size();}

      index_indirect(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      int _reg;
      uint32_type _address;
    };

    inline
    index_indirect::index_indirect(int reg, uint32_type address)
      : _reg(reg),
	_address(address)
    {
    }

    template <class Size>
    inline uint32_type
    index_indirect::address(Size, const context &c) const
    {
      uint16_type w = c.fetch_u(word(), _address);
      int xreg = w >> 12 & 0xf;
      uint32_type x = xreg >= 8 ? c.regs.a[xreg - 8] : c.regs.d[xreg];
      if (w & 0x800)
	return long_word::normal_u(c.regs.a[_reg] + long_word::get_s(x)
				   + byte::normal_s(w));
      else
	return long_word::normal_u(c.regs.a[_reg] + word::get_s(x)
				   + byte::normal_s(w));
    }

    template <class Size>
    inline typename Size::uvalue_type
    index_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    index_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline void
    index_indirect::put(Size, context &c,
		       typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, address(Size(), c), c.data_fc(), value);
    }

    template <class Size>
    inline std::string
    index_indirect::text(Size, const context &c) const
    {
      uint16_type w = c.fetch_u(word(), _address);
      int xreg = w >> 12 & 0xf;
      const char *x = xreg >= 8 ? "%a" : "%d";
      char buf[64];
      if (w & 0x800)
	std::sprintf(buf, "%%a%d@(%d,%s%d:l)/*%#lx*/", _reg, byte::normal_s(w),
		     x, xreg & 7, address(Size(), c) + 0UL);
      else
	std::sprintf(buf, "%%a%d@(%d,%s%d:w)/*%#lx*/", _reg, byte::normal_s(w),
		     x, xreg & 7, address(Size(), c) + 0UL);
      return buf;
    }

    /* Absolute short addressing.  */
    class absolute_short
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return word::size();}

      absolute_short(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      uint32_type _address;
    };

    inline
    absolute_short::absolute_short(int reg, uint32_type address)
      : _address(address)
    {
    }

    template <class Size>
    inline uint32_type
    absolute_short::address(Size, const context &c) const
    {
      return long_word::normal_u(c.fetch_s(word(), _address));
    }

    template <class Size>
    inline typename Size::uvalue_type
    absolute_short::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    absolute_short::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline void
    absolute_short::put(Size, context &c,
			typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, address(Size(), c), c.data_fc(), value);
    }

    template <class Size>
    inline std::string
    absolute_short::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%#lx:w", address(Size(), c) + 0UL);
      return buf;
    }

    /* Absolute long addressing.  */
    class absolute_long
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return long_word::size();}

      absolute_long(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      template <class Size>
      void put(Size, context &c, typename Size::uvalue_type value) const;

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      uint32_type _address;
    };

    inline
    absolute_long::absolute_long(int reg, uint32_type address)
      : _address(address)
    {
    }

    template <class Size>
    inline uint32_type
    absolute_long::address(Size, const context &c) const
    {
      return c.fetch_u(long_word(), _address);
    }

    template <class Size>
    inline typename Size::uvalue_type
    absolute_long::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    absolute_long::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline void
    absolute_long::put(Size, context &c,
		       typename Size::uvalue_type value) const
    {
      Size::put(*c.mem, address(Size(), c), c.data_fc(), value);
    }

    template <class Size>
    inline std::string
    absolute_long::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%#lx", address(Size(), c) + 0UL);
      return buf;
    }

    /* PC indirect addressing with displacement.  */
    class disp_pc_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return word::size();}

      disp_pc_indirect(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      // XXX: put is left unimplemented.

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      uint32_type _address;
    };

    inline
    disp_pc_indirect::disp_pc_indirect(int reg, uint32_type address)
      : _address(address)
    {
    }

    template <class Size>
    inline uint32_type
    disp_pc_indirect::address(Size, const context &c) const
    {
      return long_word::normal_u(_address + c.fetch_s(word(), _address));
    }

    template <class Size>
    inline typename Size::uvalue_type
    disp_pc_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.program_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    disp_pc_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.program_fc());
    }

    template <class Size>
    inline std::string
    disp_pc_indirect::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "%%pc@(%d)/*%#lx*/", c.fetch_s(word(), _address),
		   address(Size(), c) + 0UL);
      return buf;
    }

    /* PC indirect addressing with index.  */
    class index_pc_indirect
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return word::size();}

      index_pc_indirect(int reg, uint32_type address);

      template <class Size>
      uint32_type address(Size, const context &c) const;

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      // XXX: put is left unimplemented.

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
       uint32_type _address;
    };

    inline
    index_pc_indirect::index_pc_indirect(int reg, uint32_type address)
      : _address(address)
    {
    }

    template <class Size>
    inline uint32_type
    index_pc_indirect::address(Size, const context &c) const
    {
      uint16_type w = c.fetch_u(word(), _address);
      int xreg = w >> 12 & 0xf;
      uint32_type x = xreg >= 8 ? c.regs.a[xreg - 8] : c.regs.d[xreg];
      if (w & 0x800)
	return long_word::normal_u(_address + long_word::get_s(x)
				   + byte::normal_s(w));
      else
	return long_word::normal_u(_address + word::get_s(x)
				   + byte::normal_s(w));
    }

    template <class Size>
    inline typename Size::uvalue_type
    index_pc_indirect::get_u(Size, const context &c) const
    {
      return Size::get_u(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline typename Size::svalue_type
    index_pc_indirect::get_s(Size, const context &c) const
    {
      return Size::get_s(*c.mem, address(Size(), c), c.data_fc());
    }

    template <class Size>
    inline std::string
    index_pc_indirect::text(Size, const context &c) const
    {
      uint16_type w = c.fetch_u(word(), _address);
      int xreg = w >> 12 & 0xf;
      const char *x = xreg >= 8 ? "%a" : "%d";
      char buf[64];
      if (w & 0x800)
	std::sprintf(buf, "%%pc@(%d,%s%d:l)/*%#lx*/", byte::normal_s(w),
		     x, xreg & 7, address(Size(), c) + 0UL);
      else
	std::sprintf(buf, "%%pc@(%d,%s%d:w)/*%#lx*/", byte::normal_s(w),
		     x, xreg & 7, address(Size(), c) + 0UL);
      return buf;
    }

    /* Immediate addressing.  */
    class immediate
    {
    public:
      template <class Size>
      static uint32_type extension_size(Size) {return Size::aligned_size();}

      immediate(int reg, uint32_type address);

      // XXX: address is left unimplemented.

      template <class Size>
      typename Size::uvalue_type get_u(Size, const context &c) const;
      template <class Size>
      typename Size::svalue_type get_s(Size, const context &c) const;

      // XXX: put is left unimplemented.

      template <class Size>
      void finish(Size, context &c) const {}

      template <class Size>
      std::string text(Size, const context &c) const;

    protected:
      uint32_type _address;
    };

    inline
    immediate::immediate(int reg, uint32_type address)
      : _address(address)
    {
    }

    template <class Size>
    inline typename Size::uvalue_type
    immediate::get_u(Size, const context &c) const
    {
      return c.fetch_u(Size(), _address);
    }

    template <class Size>
    inline typename Size::svalue_type
    immediate::get_s(Size, const context &c) const
    {
      return c.fetch_s(Size(), _address);
    }

    template <class Size>
    inline std::string
    immediate::text(Size, const context &c) const
    {
      char buf[64];
      std::sprintf(buf, "#%#lx", get_u(Size(), c) + 0UL);
      return buf;
    }
  }
}

namespace vm68k
{
  namespace addressing
  {
    template <class Addressing, class Size> class old_addressing
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static uint32_type extension_size()
      {return Addressing::extension_size(Size());}

      old_addressing(int reg, uint32_type address)
	: addressing(reg, address) {}

      uint32_type address(const context &c) const
      {return addressing.address(Size(), c);}

      typename Size::svalue_type get(const context &c) const
      {return addressing.get_s(Size(), c);}

      void put(context &c, svalue_type value) const
      {addressing.put(Size(), c, value);}

      void finish(context &c) const {addressing.finish(Size(), c);}

      std::string text(const context &c) const
      {return addressing.text(Size(), c);}

    protected:
      Addressing addressing;
    };

    typedef old_addressing<data_register_direct, byte> byte_d_register;
    typedef old_addressing<data_register_direct, word> word_d_register;
    typedef old_addressing<data_register_direct, long_word> long_word_d_register;

    // XXX: Address register direct is not allowed for byte size.
    typedef old_addressing<address_register_direct, word> word_a_register;
    typedef old_addressing<address_register_direct, long_word> long_word_a_register;

    typedef old_addressing<indirect, byte> byte_indirect;
    typedef old_addressing<indirect, word> word_indirect;
    typedef old_addressing<indirect, long_word> long_word_indirect;

    typedef old_addressing<postinc_indirect, byte> byte_postinc_indirect;
    typedef old_addressing<postinc_indirect, word> word_postinc_indirect;
    typedef old_addressing<postinc_indirect, long_word> long_word_postinc_indirect;

    typedef old_addressing<predec_indirect, byte> byte_predec_indirect;
    typedef old_addressing<predec_indirect, word> word_predec_indirect;
    typedef old_addressing<predec_indirect, long_word> long_word_predec_indirect;

    typedef old_addressing<disp_indirect, byte> byte_disp_indirect;
    typedef old_addressing<disp_indirect, word> word_disp_indirect;
    typedef old_addressing<disp_indirect, long_word> long_word_disp_indirect;

    typedef old_addressing<index_indirect, byte> byte_index_indirect;
    typedef old_addressing<index_indirect, word> word_index_indirect;
    typedef old_addressing<index_indirect, long_word> long_word_index_indirect;

    typedef old_addressing<absolute_short, byte> byte_abs_short;
    typedef old_addressing<absolute_short, word> word_abs_short;
    typedef old_addressing<absolute_short, long_word> long_word_abs_short;

    typedef old_addressing<absolute_long, byte> byte_abs_long;
    typedef old_addressing<absolute_long, word> word_abs_long;
    typedef old_addressing<absolute_long, long_word> long_word_abs_long;

    typedef old_addressing<disp_pc_indirect, byte> byte_disp_pc_indirect;
    typedef old_addressing<disp_pc_indirect, word> word_disp_pc_indirect;
    typedef old_addressing<disp_pc_indirect, long_word> long_word_disp_pc_indirect;

    typedef old_addressing<index_pc_indirect, byte> byte_index_pc_indirect;
    typedef old_addressing<index_pc_indirect, word> word_index_pc_indirect;
    typedef old_addressing<index_pc_indirect, long_word> long_word_index_pc_indirect;

    typedef old_addressing<immediate, byte> byte_immediate;
    typedef old_addressing<immediate, word> word_immediate;
    typedef old_addressing<immediate, long_word> long_word_immediate;
  }
}

#endif /* not _VM68K_ADDRESSING */