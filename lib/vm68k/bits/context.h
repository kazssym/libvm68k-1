/* -*-c++-*- */
/* context - context unit private header for Virtual M68000 Toolkit
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

#ifndef _VM68K_CONTEXT_H
#define _VM68K_CONTEXT_H 1

#include <vector>
#include <queue>

namespace vx68k
{
  /* Abstruct base class for condition testers.  */
  class condition_tester
  {
  public:
    virtual ~condition_tester () {}

  public:
    virtual bool ls(const int_least32_t *) const;
    virtual bool cs(const int_least32_t *) const = 0;
    virtual bool eq(const int_least32_t *) const = 0;
    virtual bool mi(const int_least32_t *) const = 0;
    virtual bool lt(const int_least32_t *) const = 0;
    virtual bool le(const int_least32_t *) const;
    virtual unsigned int x(const int_least32_t *) const;
  };

  inline bool
  condition_tester::ls(const int_least32_t *v) const
  {
    return this->cs(v) || this->eq(v);
  }

  inline bool
  condition_tester::le(const int_least32_t *v) const
  {
    return this->eq(v) || this->lt(v);
  }

  inline unsigned int
  condition_tester::x(const int_least32_t *v) const
  {
    return this->cs(v);
  }

  class bitset_condition_tester: public condition_tester
  {
  public:
    bool cs(const int_least32_t *) const;
    bool eq(const int_least32_t *) const;
    bool mi(const int_least32_t *) const;
    bool lt(const int_least32_t *) const;
  };

  /* Status register.  */
  class VM68K_PUBLIC _VM68K_DEPRECATED status_register
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
    int_least32_t cc_values[3];
    const condition_tester *x_eval;
    int_least32_t x_values[3];
    uint_least16_t value;

  public:
    status_register();

  public:
    operator uint_fast16_t() const;
    status_register &operator=(uint_fast16_t v)
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
    void set_cc(int_fast32_t r)
    {
      cc_eval = general_condition_tester;
      cc_values[0] = r;
    }

    /* Sets the condition codes as ADD.  */
    void set_cc_as_add(int_fast32_t r, int_fast32_t d, int_fast32_t s)
    {
      x_eval = cc_eval = add_condition_tester;
      x_values[0] = cc_values[0] = r;
      x_values[1] = cc_values[1] = d;
      x_values[2] = cc_values[2] = s;
    }

    void set_cc_cmp(int_fast32_t, int_fast32_t, int_fast32_t);
    void set_cc_sub(int_fast32_t, int_fast32_t, int_fast32_t);
    void set_cc_asr(int_fast32_t, int_fast32_t, int_fast32_t);
    void set_cc_lsr(int_fast32_t r, int_fast32_t d, int_fast32_t s)
      {set_cc_asr(r, d, s);}
    void set_cc_lsl(int_fast32_t, int_fast32_t, int_fast32_t);

  public:
    /* Returns whether supervisor state.  */
    bool supervisor_state() const
      {return (value & S) != 0;}

    /* Sets the S bit.  */
    void set_s_bit(bool s)
      {if (s) value |= S; else value &= ~S;}
  };

  /* Context of execution.  A context represents all the state of
     execution.  See also `class processor'.  */
  class VM68K_PUBLIC vm68k_context
  {
  public:
    explicit vm68k_context (vm68k_bus *bus);
    ~vm68k_context ();

  public:
    static vm68k_context *current_context ();
    static void set_current_context (vm68k_context *context);

  public:
    static const int D0 = 0;
    static const int A0 = 8;
    static const int SP = A0 + 7;

    template<class Size>
    typename Size::udata_type read_reg_unsigned (const Size &,
                                                 int regno) const
    {
      return Size::read_unsigned (_reg[regno]);
    }

    template<class Size>
    typename Size::data_type read_reg (const Size &,
                                       int regno) const
    {
      return Size::read (_reg[regno]);
    }

    template<class Size>
    void write_reg (const Size &,
                    int regno, typename Size::udata_type value)
    {
      Size::write (_reg[regno], value);
    }

    /* Returns true if supervisor state.  */
    bool super () const
    {
      return (_status_high & S) == S;
    }

    /* Sets the supervisor state to STATE.  */
    void set_super (bool state);

    /* Returns the value of the status register.  */
    uint_fast16_t status () const
    {
      return _status_high | (_status & 0xffU);
    }

    /* Sets the status register.  */
    void set_status (uint_fast16_t value);

  private:
    static const uint_fast16_t S = 1 << 13;
    union
    {
      uint_least32_t _reg[8 + 8];
      struct
      {
        uint_least32_t d0, d1, d2, d3, d4, d5, d6, d7;
        uint_least32_t a0, a1, a2, a3, a4, a5, a6, sp;
      } _named_reg;
    };
    uint_least32_t _usp, _ssp;
    uint_least16_t _status_high;
  public: /* FIXME temporarily public */
    /*_VM68K_DEPRECATED*/ status_register _status;

  public:
    template<class Size>
    typename Size::udata_type load_unsigned (const Size &,
                                             vm68k_address_t addr) const
    {
      return Size::read_unsigned (_bus, dfc_cache, addr);
    }

    template<class Size>
    typename Size::data_type load (const Size &, vm68k_address_t addr) const
    {
      return Size::read (_bus, dfc_cache, addr);
    }

    template<class Size>
    void store (const Size &, vm68k_address_t addr,
                typename Size::udata_type value)
    {
      return Size::write (_bus, dfc_cache, addr, value);
    }

    template<class Size>
    typename Size::udata_type pop_unsigned (const Size &) const
    {
      typename Size::uint_type value =
        Size::read_unsigned (_bus, dfc_cache, _named_reg.sp);
      _named_reg.sp += Size::aligned_data_size ();
      return value;
    }

    template<class Size>
    typename Size::data_type pop (const Size &) const
    {
      typename Size::data_type value =
        Size::read (_bus, dfc_cache, _named_reg.sp);
      _named_reg.sp += Size::aligned_data_size ();
      return value;
    }

    template<class Size>
    void push (const Size &, typename Size::udata_type value)
    {
      _named_reg.sp -= Size::aligned_data_size ();
      Size::write (_bus, dfc_cache, _named_reg.sp);
    }

    template<class Size>
    typename Size::udata_type fetch_unsigned (const Size &, 
                                              vm68k_address_t addr) const
    {
      return Size::read_inst_unsigned (_bus, pfc_cache, addr);
    }

    template<class Size>
    typename Size::data_type fetch (const Size &,
                                    vm68k_address_t addr) const
    {
      return Size::read_inst (_bus, pfc_cache, addr);
    }

  private:
    vm68k_bus *_bus;

    /* Cache values for program and data FC's.  */
    vm68k_bus::function_code dfc_cache, pfc_cache;

  private:			// interrupt
    /* True if the thread in this context is interrupted.  */
    bool a_interrupted;

    std::queue<uint_least8_t> interrupt_queue[7];

  public:			// interrupt
    /* Returns true if the thread in this context is interrupted.  */
    bool interrupted () const
    {
      return a_interrupted;
    }

    /* Interrupts.  */
    void interrupt (int priority, uint_fast8_t vecno);
  };
}

#endif
