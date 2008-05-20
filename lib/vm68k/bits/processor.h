/* -*-c++-*-
 * processor - processor unit private header for Virtual M68000 Toolkit
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

#ifndef _VM68K_PROCESSOR_H
#define _VM68K_PROCESSOR_H 1

#include <exception>

namespace vx68k
{
  /* Base class of processor exceptions.  */
  class VM68K_PUBLIC vm68k_exception : public std::exception
  {
  protected:
    explicit vm68k_exception (vm68k_address_t pc);

  public:
    vm68k_address_t pc () const throw ()
    {
      return _pc;
    }

    virtual uint_fast16_t vecno () const throw () = 0;

  private:
    vm68k_address_t _pc;
  };

  /* Bus error exception.  */
  class VM68K_PUBLIC vm68k_bus_error_exception : public vm68k_exception
  {
  public:
    vm68k_bus_error_exception (vm68k_address_t pc,
                               const vm68k_bus_error &source)
      : vm68k_exception (pc),
        _source (source)
    {
    }

    const vm68k_bus_error &source () const throw ()
    {
      return _source;
    }

    uint_fast16_t vecno () const throw ()
    {
      return 2;
    }

  private:
    vm68k_bus_error _source;
  };

  /* Address error exception.  */
  class VM68K_PUBLIC vm68k_address_error_exception : public vm68k_exception
  {
  public:
    vm68k_address_error_exception (vm68k_address_t pc,
                                   const vm68k_address_error &source)
      : vm68k_exception(pc),
        _source (source)
    {
    }

    const vm68k_address_error &source () const throw ()
    {
      return _source;
    }

    uint_fast16_t vecno () const throw ()
    {
      return 3;
    }

  private:
    vm68k_address_error _source;
  };

  /* Illegal instruction exception.  */
  struct VM68K_PUBLIC vm68k_illegal_instruction_exception : vm68k_exception
  {
    explicit vm68k_illegal_instruction_exception (vm68k_address_t pc)
      : vm68k_exception (pc)
    {
    }

    uint_fast16_t vecno () const throw ()
    {
      return 4;
    }
  };

  /* Zero divide exception.  */
  struct zero_divide_exception: vm68k_exception
  {
    explicit zero_divide_exception(vm68k_address_t pc);
    uint_fast16_t vecno () const throw () {return 5;}
  };

  inline
  zero_divide_exception::zero_divide_exception(vm68k_address_t pc)
    : vm68k_exception(pc)
  {
  }

  /* Privilege violation exception.  */
  struct privilege_violation_exception: vm68k_exception
  {
    explicit privilege_violation_exception(vm68k_address_t pc);
    uint_fast16_t vecno () const throw () {return 8;}
  };

  /* Constructor.  */
  inline
  privilege_violation_exception::privilege_violation_exception(vm68k_address_t pc)
    : vm68k_exception (pc)
  {
  }

  class VM68K_PUBLIC vm68k_instruction
  {
  public:
    /* Type of an instruction handler.  */
    typedef vm68k_address_t (*function) (vm68k_address_t pc,
                                         uint_fast16_t w,
                                         vm68k_context *c);

  public:
    friend bool operator== (const vm68k_instruction &x,
                            const vm68k_instruction &y)
    {
      return x._func == y._func;
    }

  public:
    vm68k_instruction ();
    vm68k_instruction (function func);

  public:
    vm68k_address_t operator() (vm68k_address_t pc, uint_fast16_t w,
                                vm68k_context *c) const
    {
      return _func (pc, w, c);
    }

  private:
    function _func;
  };

  inline bool operator!= (const vm68k_instruction &x,
                          const vm68k_instruction &y)
  {
    return !(x == y);
  }

  /* Decodes and executes an instruction sequence.  */
  class VM68K_PUBLIC vm68k_instruction_decoder
  {
  public:
    struct spec
    {
      uint_least16_t code;
      uint_least16_t mask;
      vm68k_instruction::function func;
    };

  private:
    static void insert_inst0 (vm68k_instruction_decoder *p);
    static void insert_inst1 (vm68k_instruction_decoder *p);
    static void insert_inst2 (vm68k_instruction_decoder *p);
    static void insert_inst3 (vm68k_instruction_decoder *p);
    static void insert_inst4 (vm68k_instruction_decoder *p);
    static void insert_inst5 (vm68k_instruction_decoder *p);
    static void insert_inst6 (vm68k_instruction_decoder *p);
    static void insert_inst7 (vm68k_instruction_decoder *p);
    static void insert_inst8 (vm68k_instruction_decoder *p);
    static void insert_inst9 (vm68k_instruction_decoder *p);
    static void insert_inst10 (vm68k_instruction_decoder *p);
    static void insert_inst11 (vm68k_instruction_decoder *p);
    static void insert_inst12 (vm68k_instruction_decoder *p);
    static void insert_inst13 (vm68k_instruction_decoder *p);
    static void insert_inst14 (vm68k_instruction_decoder *p);
    static void insert_inst15 (vm68k_instruction_decoder *p);

  public:
    vm68k_instruction_decoder ();
    virtual ~vm68k_instruction_decoder ();

  public:
    /* Sets an instruction for an operation word.  The old value is
       returned.  */
    void insert (uint_fast16_t code, vm68k_instruction::function i);
    void insert (const spec &s);

    template<typename InputIterator>
    void insert (InputIterator first, InputIterator last)
    {
      while (first != last)
        {
          this->insert (*first++);
        }
    }

    /* Starts the program.  */
    vm68k_address_t run (vm68k_address_t pc, vm68k_context &c) const
      throw (vm68k_exception);

  protected:
    /* Dispatches for instruction handlers.  */
    vm68k_address_t dispatch (vm68k_address_t pc, uint_fast16_t w,
                        vm68k_context *c) const
    {
      return _instruction[w] (pc, w, c);
    }

  private:
    vm68k_instruction _instruction[0x10000];
  };
}

#endif
