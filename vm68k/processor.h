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

#ifndef _VM68K_PROCESSOR_H
#define _VM68K_PROCESSOR_H 1

#include <vm68k/size.h>		// Provisionally.
#include <vm68k/memory.h>
#include <queue>
#include <vector>
#include <utility>

namespace vm68k
{
  using namespace std;

  /* Base class of processor exceptions.  */
  class processor_exception: public exception
  {
  private:
    uint32_type _pc;

  protected:
    explicit processor_exception(uint32_type pc)
      : _pc(pc) {}

  public:
    uint32_type pc() const throw () {return _pc;}
  };

  /* Illegal instruction exception.  */
  struct illegal_instruction_exception: processor_exception
  {
    explicit illegal_instruction_exception(uint32_type pc)
      : processor_exception(pc) {}
  };

  /* Zero divide exception.  */
  struct zero_divide_exception: processor_exception
  {
    explicit zero_divide_exception(uint32_type pc)
      : processor_exception(pc) {}
  };

  /* Privilege violation exception.  */
  struct privilege_violation_exception: processor_exception
  {
    explicit privilege_violation_exception(uint32_type pc)
      : processor_exception(pc) {}
  };
  
  /* Abstruct base class for condition testers.  */
  class condition_tester
  {
  public:
    virtual bool ls(const sint32_type *) const;
    virtual bool cs(const sint32_type *) const = 0;
    virtual bool eq(const sint32_type *) const = 0;
    virtual bool mi(const sint32_type *) const = 0;
    virtual bool lt(const sint32_type *) const = 0;
    virtual bool le(const sint32_type *) const;
    virtual unsigned int x(const sint32_type *) const;
  };

  inline bool
  condition_tester::ls(const sint32_type *v) const
  {
    return this->cs(v) || this->eq(v);
  }

  inline bool
  condition_tester::le(const sint32_type *v) const
  {
    return this->eq(v) || this->lt(v);
  }

  inline unsigned int
  condition_tester::x(const sint32_type *v) const
  {
    return this->cs(v);
  }

  class bitset_condition_tester: public condition_tester
  {
  public:
    bool cs(const sint32_type *) const;
    bool eq(const sint32_type *) const;
    bool mi(const sint32_type *) const;
    bool lt(const sint32_type *) const;
  };
  
  /* Status register.  */
  class condition_code
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
    sint32_type cc_values[3];
    const condition_tester *x_eval;
    sint32_type x_values[3];
    uint16_type value;

  public:
    condition_code();

  public:
    operator uint16_type() const;
    condition_code &operator=(uint16_type v)
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
    void set_cc(sint32_type r)
    {
      cc_eval = general_condition_tester;
      cc_values[0] = r;
    }

    /* Sets the condition codes as ADD.  */
    void set_cc_as_add(sint32_type r, sint32_type d, sint32_type s)
    {
      x_eval = cc_eval = add_condition_tester;
      x_values[0] = cc_values[0] = r;
      x_values[1] = cc_values[1] = d;
      x_values[2] = cc_values[2] = s;
    }

    void set_cc_cmp(sint32_type, sint32_type, sint32_type);
    void set_cc_sub(sint32_type, sint32_type, sint32_type);
    void set_cc_asr(sint32_type, sint32_type, sint32_type);
    void set_cc_lsr(sint32_type r, sint32_type d, sint32_type s)
      {set_cc_asr(r, d, s);}
    void set_cc_lsl(sint32_type, sint32_type, sint32_type);

  public:
    /* Returns whether supervisor state.  */
    bool supervisor_state() const
      {return (value & S) != 0;}

    /* Sets the S bit.  */
    void set_s_bit(bool s)
      {if (s) value |= S; else value &= ~S;}
  };

  typedef condition_code status_register;

  /* CPU registers (mc68000).  */
  struct registers
  {
    uint32_type d[8];		/* %d0-%d7 */
    uint32_type a[8];		/* %a0-%a6/%sp */
    condition_code ccr;
    uint32_type usp;
    uint32_type ssp;

    template <class Size>
    typename Size::svalue_type data_register(unsigned int regno, Size)
    {return Size::get(d[regno]);}
    template <class Size>
    void set_data_register(unsigned int regno, Size,
			   typename Size::uvalue_type value)
    {Size::put(d[regno], value);}
  };

  /* Context of execution.  A context represents all the state of
     execution.  See also `class processor'.  */
  class context
  {
  public:
    registers regs;
    memory_map *mem;

  private:
    /* Cache values for program and data FC's.  */
    memory::function_code pfc_cache, dfc_cache;

  private:			// interrupt
    /* True if the thread in this context is interrupted.  */
    bool a_interrupted;

    vector<queue<unsigned int> > interrupt_queues;

  public:
    explicit context(memory_map *);

  public:
    /* Returns true if supervisor state.  */
    bool supervisor_state() const
    {return regs.ccr.supervisor_state();}

    /* Sets the supervisor state to STATE.  */
    void set_supervisor_state(bool state);

    /* Returns the value of the status register.  */
    uint16_type sr() const;

    /* Sets the status register.  */
    void set_sr(uint16_type value);

  public:
    /* Returns the FC for program in the current state.  */
    memory::function_code program_fc() const {return pfc_cache;}

    /* Returns the FC for data in the current state.  */
    memory::function_code data_fc() const {return dfc_cache;}

  public:
    template <class Size>
    typename Size::uvalue_type fetch_u(Size, uint32_type address) const;
    template <class Size>
    typename Size::svalue_type fetch_s(Size, uint32_type address) const;

  public:			// interrupt
    /* Returns true if the thread in this context is interrupted.  */
    bool interrupted() const
    {return a_interrupted;}

    /* Interrupts.  */
    void interrupt(int prio, unsigned int vecno);

    /* XXX This should be in class processor.  */
    /* Handles interrupts.  */
    uint32_type handle_interrupts(uint32_type pc);
  };

  template <class Size> inline typename Size::uvalue_type
  context::fetch_u(Size, uint32_type address) const
  {
    return Size::uget_unchecked(*mem, program_fc(), address);
  }

  template <> inline byte_size::uvalue_type
  context::fetch_u(byte_size, uint32_type address) const
  {
    return byte_size::uvalue(word_size::uget_unchecked(*mem, program_fc(),
						       address));
  }

  template <class Size> inline typename Size::svalue_type
  context::fetch_s(Size, uint32_type address) const
  {
    return Size::get_unchecked(*mem, program_fc(), address);
  }

  template <> inline byte_size::svalue_type
  context::fetch_s(byte_size, uint32_type address) const
  {
    return byte_size::svalue(word_size::uget_unchecked(*mem, program_fc(),
						       address));
  }

  /* Decodes and executes an instruction sequence.  */
  class processor
  {
  public:
    /* Type of an instruction handler.  */
    typedef uint32_type (*instruction_handler)
      (uint32_type pc, context &, uint16_type w, unsigned long d);

    /* Type of an instruction.  */
    typedef pair<instruction_handler, unsigned long> instruction_type;

  public:
    /* Raises an illegal instruction exception.  */
    static uint32_type illegal(uint32_type pc, context &,
			       uint16_type w, unsigned long d);

  private:
    vector<instruction_type> instructions;

  public:
    processor();

  public:
    /* Sets an instruction for an operation word.  The old value is
       returned.  */
    instruction_type set_instruction(uint16_type op, const instruction_type &i)
    {
      op &= 0xffffu;
      instruction_type old_value = instructions[op];
      instructions[op] = i;
      return old_value;
    }

    /* Sets an instruction to operation codes.  */
    void set_instruction(int op, int mask, const instruction_type &i);

    void set_instruction(int op, int mask, instruction_handler h)
      {set_instruction(op, mask, instruction_type(h, 0));}

  protected:
    /* Dispatches for instruction handlers.  */
    uint32_type dispatch(uint32_type pc, context &c, uint16_type w) const;

  public:
    /* Steps one instruction.  */
    uint32_type step(uint32_type pc, context &c) const;

    /* Starts the program.  */
    uint32_type run(uint32_type pc, context &c) const;
  };

  inline uint32_type
  processor::dispatch(uint32_type pc, context &c, uint16_type w) const
  {
    w &= 0xffff;
    return instructions[w].first(pc, c, w, instructions[w].second);
  }

  inline uint32_type
  processor::step(uint32_type pc, context &c) const
  {
    return dispatch(pc, c, c.fetch_u(word_size(), pc));
  }
}

#endif /* not _VM68K_PROCESSOR_H */
