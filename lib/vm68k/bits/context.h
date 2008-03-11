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

#ifndef _VM68K_CONTEXT
#define _VM68K_CONTEXT 1

#include <vector>
#include <queue>
#include <vm68k/bus>

namespace vx68k_m68k
{
  /* Abstruct base class for condition testers.  */
  class condition_tester
  {
  public:
    virtual ~condition_tester () {}

  public:
    virtual bool ls(const data32_t *) const;
    virtual bool cs(const data32_t *) const = 0;
    virtual bool eq(const data32_t *) const = 0;
    virtual bool mi(const data32_t *) const = 0;
    virtual bool lt(const data32_t *) const = 0;
    virtual bool le(const data32_t *) const;
    virtual unsigned int x(const data32_t *) const;
  };

  inline bool
  condition_tester::ls(const data32_t *v) const
  {
    return this->cs(v) || this->eq(v);
  }

  inline bool
  condition_tester::le(const data32_t *v) const
  {
    return this->eq(v) || this->lt(v);
  }

  inline unsigned int
  condition_tester::x(const data32_t *v) const
  {
    return this->cs(v);
  }

  class bitset_condition_tester: public condition_tester
  {
  public:
    bool cs(const data32_t *) const;
    bool eq(const data32_t *) const;
    bool mi(const data32_t *) const;
    bool lt(const data32_t *) const;
  };

  /* Status register.  */
  class _VM68K_PUBLIC status_register
  {
  protected:
    enum
    {S = 1 << 13};

  private:			// Condition testers
    static const bitset_condition_tester bitset_tester;
    static const condition_tester *const general_condition_tester;
    static const condition_tester *const add_condition_tester;

  private:
    const condition_tester *cc_eval;
    data32_t cc_values[3];
    const condition_tester *x_eval;
    data32_t x_values[3];
    udata16_t value;

  public:
    status_register();

  public:
    operator udata16_t() const;
    status_register &operator=(udata16_t v)
    {
      value = v & 0xff00;
      x_eval = cc_eval = &bitset_tester;
      x_values[0] = cc_values[0] = v;
      return *this;
    }

  public:
    bool hi() const {return !cc_eval->ls(cc_values);}
    bool ls() const {return  cc_eval->ls(cc_values);}
    bool cc() const {return !cc_eval->cs(cc_values);}
    bool cs() const {return  cc_eval->cs(cc_values);}
    bool ne() const {return !cc_eval->eq(cc_values);}
    bool eq() const {return  cc_eval->eq(cc_values);}
    bool pl() const {return !cc_eval->mi(cc_values);}
    bool mi() const {return  cc_eval->mi(cc_values);}
    bool ge() const {return !cc_eval->lt(cc_values);}
    bool lt() const {return  cc_eval->lt(cc_values);}
    bool gt() const {return !cc_eval->le(cc_values);}
    bool le() const {return  cc_eval->le(cc_values);}

  public:
    int x() const
    {return x_eval->cs(x_values) ? 1 : 0;}

  public:
    /* Sets the condition codes by a result.  */
    void set_cc(data_fast32_t r)
    {
      cc_eval = general_condition_tester;
      cc_values[0] = r;
    }

    /* Sets the condition codes as ADD.  */
    void set_cc_as_add(data_fast32_t r, data_fast32_t d, data_fast32_t s)
    {
      x_eval = cc_eval = add_condition_tester;
      x_values[0] = cc_values[0] = r;
      x_values[1] = cc_values[1] = d;
      x_values[2] = cc_values[2] = s;
    }

    void set_cc_cmp(data_fast32_t, data_fast32_t, data_fast32_t);
    void set_cc_sub(data_fast32_t, data_fast32_t, data_fast32_t);
    void set_cc_asr(data_fast32_t, data_fast32_t, data_fast32_t);
    void set_cc_lsr(data_fast32_t r, data_fast32_t d, data_fast32_t s)
      {set_cc_asr(r, d, s);}
    void set_cc_lsl(data_fast32_t, data_fast32_t, data_fast32_t);

  public:
    /* Returns whether supervisor state.  */
    bool supervisor_state() const
      {return (value & S) != 0;}

    /* Sets the S bit.  */
    void set_s_bit(bool s)
      {if (s) value |= S; else value &= ~S;}
  };

  enum register_name
  {
    D0 = 0, D1, D2, D3, D4, D5, D6, D7,
    A0 = 8, A1, A2, A3, A4, A5, A6, SP,
    USP, SSP
  };

  /* Context of execution.  A context represents all the state of
     execution.  See also `class processor'.  */
  class _VM68K_PUBLIC context
  {
  public:
    explicit context (system_bus *bus);

  public:
    static context *current_context ();
    static void switch_context (context *new_context);

  public:
    template <class Size>
    typename Size::udata_type read_reg_unsigned (const Size &, int regno) const
    {
      return Size::read_unsigned (_reg[regno]);
    }

    template <class Size>
    typename Size::data_type read_reg (const Size &, int regno) const
    {
      return Size::read (_reg[regno]);
    }

    template <class Size>
    void write_reg (const Size &, int regno, typename Size::udata_type value)
    {
      Size::write (_reg[regno], value);
    }

    /* Returns true if supervisor state.  */
    bool super () const
    {
      return _status.supervisor_state ();
    }

    /* Sets the supervisor state to STATE.  */
    void set_super (bool state);

    /* Returns the value of the status register.  */
    udata_fast16_t status () const
    {
      return _status;
    }

    /* Sets the status register.  */
    void set_status (udata_fast16_t value);

  private:
    union
    {
      udata32_t _reg[16 + 2];
      struct
      {
        udata32_t d0, d1, d2, d3, d4, d5, d6, d7;
        udata32_t a0, a1, a2, a3, a4, a5, a6, sp;
        udata32_t usp, ssp;
      } _named_reg;
    };
  public: /* FIXME temporarily public */
    status_register _status;

  public:
    template <class Size>
    typename Size::udata_type load_unsigned (Size, address_t address) const
    {
      return Size::read_unsigned (_bus, dfc_cache, address);
    }

    template <class Size>
    typename Size::data_type load (Size, address_t address) const
    {
      return Size::read (_bus, dfc_cache, address);
    }

    template <class Size>
    void store (Size, address_t address, typename Size::udata_type value) const
    {
      return Size::write (_bus, dfc_cache, address, value);
    }

    template <class Size>
    typename Size::udata_type fetch_unsigned (Size, address_t address) const
    {
      return Size::read_i_unsigned (_bus, pfc_cache, address);
    }

    template <class Size>
    typename Size::data_type fetch (Size, address_t address) const
    {
      return Size::read_i (_bus, pfc_cache, address);
    }

  private:
    system_bus *_bus;

    /* Cache values for program and data FC's.  */
    function_code dfc_cache, pfc_cache;

  private:			// interrupt
    /* True if the thread in this context is interrupted.  */
    bool a_interrupted;

    std::queue<udata8_t> interrupt_queue[7];

  public:			// interrupt
    /* Returns true if the thread in this context is interrupted.  */
    bool interrupted () const
    {
      return a_interrupted;
    }

    /* Interrupts.  */
    void interrupt (int priority, udata_fast8_t vecno);
  };
}

#endif
