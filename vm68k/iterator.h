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

#ifndef __VM68K_ITERATOR_H
#define __VM68K_ITERATOR_H 1

#include <vm68k/types.h>
#include <iterator>

namespace vm68k
{
  using namespace std;

  /* Helper iterator for 16-bit values.  This iterator reads/writes two
     bytes at once to handle a 16-bit value.  */
  template <class T>
  class basic_uint16_iterator: random_access_iterator<uint16_type, ptrdiff_t>
  {
  protected:
    class ref
    {
    private:
      T bp;

    public:
      ref(T ptr): bp(ptr) {}

    public:
      ref &operator=(uint16_type);
      operator uint16_type() const;
    };

  private:
    T bp;

  public:
    basic_uint16_iterator(T ptr): bp(ptr) {}
    template <class U> explicit basic_uint16_iterator(U ptr)
      : bp(static_cast<T>(ptr)) {}

  public:
    uint16_type operator*() const {return ref(bp);}
    ref operator*() {return ref(bp);}

    basic_uint16_iterator &operator++();
    basic_uint16_iterator operator++(int);

    basic_uint16_iterator &operator--();
    basic_uint16_iterator operator--(int);

    basic_uint16_iterator &operator+=(ptrdiff_t n);
    basic_uint16_iterator operator+(ptrdiff_t n) const
    {return basic_uint16_iterator(*this) += n;}

    basic_uint16_iterator &operator-=(ptrdiff_t n);
    basic_uint16_iterator operator-(ptrdiff_t n) const
    {return basic_uint16_iterator(*this) -= n;}

    uint16_type operator[](ptrdiff_t n) const {return *(*this + n);}
    ref operator[](ptrdiff_t n) {return *(*this + n);}

  public:
    operator T() const {return bp;}
  };

  /* Helper iterator for 32-bit value.  This iterator reads/writes four
     bytes at once to handle a 32-bit value.  */
  template <class T>
  class basic_uint32_iterator: random_access_iterator<uint32_type, ptrdiff_t>
  {
  protected:
    class ref
    {
    private:
      T bp;

    public:
      ref(T ptr): bp(ptr) {}

    public:
      ref &operator=(uint32_type);
      operator uint32_type() const;
    };

  private:
    T bp;

  public:
    basic_uint32_iterator(T ptr): bp(ptr) {}
    template <class U> explicit basic_uint32_iterator(U ptr)
      : bp(static_cast<T >(ptr)) {}

  public:
    uint32_type operator*() const {return ref(bp);}
    ref operator*() {return ref(bp);}

    basic_uint32_iterator &operator++();
    basic_uint32_iterator operator++(int);

    basic_uint32_iterator &operator--();
    basic_uint32_iterator operator--(int);

    basic_uint32_iterator &operator+=(ptrdiff_t n);
    basic_uint32_iterator operator+(ptrdiff_t n) const
    {return basic_uint32_iterator(*this) += n;}

    basic_uint32_iterator &operator-=(ptrdiff_t n);
    basic_uint32_iterator operator-(ptrdiff_t n) const
    {return basic_uint32_iterator(*this) -= n;}

    uint32_type operator[](ptrdiff_t n) const {return *(*this + n);}
    ref operator[](ptrdiff_t n) {return *(*this + n);}

  public:
    operator T() const {return bp;}
  };

  typedef basic_uint16_iterator<unsigned char *> uint16_iterator;
  typedef basic_uint16_iterator<const unsigned char *> const_uint16_iterator;
  typedef basic_uint32_iterator<unsigned char *> uint32_iterator;
  typedef basic_uint32_iterator<const unsigned char *> const_uint32_iterator;
  
  template <class T>
  inline basic_uint16_iterator<T>::ref &
  basic_uint16_iterator<T>::ref::operator=(uint16_type value)
  {
    bp[0] = value >> 8;
    bp[1] = value;
    return *this;
  }

  template <class T>
  inline
  basic_uint16_iterator<T>::ref::operator uint16_type() const
  {
    return uint16_type(bp[0] & 0xff) << 8 | bp[1] & 0xff;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator++()
  {
    bp += 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  basic_uint16_iterator<T>::operator++(int)
  {
    basic_uint16_iterator<T> old = *this;
    ++(*this);
    return old;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator--()
  {
    bp -= 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  basic_uint16_iterator<T>::operator--(int)
  {
    basic_uint16_iterator<T> old = *this;
    --(*this);
    return old;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator+=(ptrdiff_t n)
  {
    bp += n * 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator-=(ptrdiff_t n)
  {
    bp -= n * 2;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>::ref &
  basic_uint32_iterator<T>::ref::operator=(uint32_type value)
  {
    bp[0] = value >> 24;
    bp[1] = value >> 16;
    bp[2] = value >>  8;
    bp[3] = value;
    return *this;
  }

  template <class T>
  inline
  basic_uint32_iterator<T>::ref::operator uint32_type() const
  {
    return (uint32_type(bp[0] & 0xff) << 24 | uint32_type(bp[1] & 0xff) << 16
	    | uint32_type(bp[2] & 0xff) << 8 | uint32_type(bp[3] & 0xff));
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator++()
  {
    bp += 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  basic_uint32_iterator<T>::operator++(int)
  {
    basic_uint32_iterator<T> old = *this;
    ++(*this);
    return old;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator--()
  {
    bp -= 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  basic_uint32_iterator<T>::operator--(int)
  {
    basic_uint32_iterator<T> old = *this;
    --(*this);
    return old;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator+=(ptrdiff_t n)
  {
    bp += n * 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator-=(ptrdiff_t n)
  {
    bp -= n * 4;
    return *this;
  }
}

#endif /* not __VM68K_ITERATOR_H */
