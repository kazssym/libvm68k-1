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

#include <vm68k/type>
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
      T p;

    public:
      explicit ref(const T &ptr);

    public:
      ref &operator=(const uint16_type &value);
      operator uint16_type() const;
    };

  private:
    T p;

  public:
    basic_uint16_iterator(const T &ptr);
    template <class U>
    basic_uint16_iterator(const basic_uint16_iterator<U> &x);

  public:
    T base() const {return p;}

  public:
    uint16_type operator*() const {return ref(p);}
    ref operator*() {return ref(p);}

    basic_uint16_iterator<T> &operator++();
    basic_uint16_iterator<T> operator++(int x);

    basic_uint16_iterator<T> &operator--();
    basic_uint16_iterator<T> operator--(int x);

    basic_uint16_iterator<T> &operator+=(ptrdiff_t n);
    basic_uint16_iterator<T> &operator-=(ptrdiff_t n);
    uint16_type operator[](ptrdiff_t n) const;
    ref operator[](ptrdiff_t n);
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
      T p;

    public:
      explicit ref(const T &ptr);

    public:
      ref &operator=(const uint32_type &value);
      operator uint32_type() const;
    };

  private:
    T p;

  public:
    basic_uint32_iterator(const T &ptr);
    template <class U>
    basic_uint32_iterator(const basic_uint32_iterator<U> &x);

  public:
    T base() const {return p;}

  public:
    uint32_type operator*() const {return ref(p);}
    ref operator*() {return ref(p);}

    basic_uint32_iterator<T> &operator++();
    basic_uint32_iterator<T> operator++(int x);

    basic_uint32_iterator<T> &operator--();
    basic_uint32_iterator<T> operator--(int x);

    basic_uint32_iterator<T> &operator+=(ptrdiff_t n);
    basic_uint32_iterator<T> &operator-=(ptrdiff_t n);
    uint32_type operator[](ptrdiff_t n) const;
    ref operator[](ptrdiff_t n);
  };

  typedef basic_uint16_iterator<unsigned char *> uint16_iterator;
  typedef basic_uint16_iterator<const unsigned char *> const_uint16_iterator;
  typedef basic_uint32_iterator<unsigned char *> uint32_iterator;
  typedef basic_uint32_iterator<const unsigned char *> const_uint32_iterator;
  
  /* Implementation of `class basic_uint16_iterator'.  */

  template <class T>
  inline basic_uint16_iterator<T>::ref &
  basic_uint16_iterator<T>::ref::operator=(const uint16_type &value)
  {
    p[0] = value >> 8;
    p[1] = value;
    return *this;
  }

  template <class T>
  inline
  basic_uint16_iterator<T>::ref::operator uint16_type() const
  {
    return uint16_type(p[0] & 0xff) << 8 | p[1] & 0xff;
  }

  template <class T>
  inline
  basic_uint16_iterator<T>::ref::ref(const T &ptr)
    : p(ptr)
  {
  }

  template <class T>
  inline bool
  operator==(const basic_uint16_iterator<T> &x,
	     const basic_uint16_iterator<T> &y)
  {
    return x.base() == y.base();
  }

  template <class T>
  inline bool
  operator!=(const basic_uint16_iterator<T> &x,
	     const basic_uint16_iterator<T> &y)
  {
    return !(x == y);
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator++()
  {
    p += 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  basic_uint16_iterator<T>::operator++(int x)
  {
    basic_uint16_iterator<T> old = *this;
    ++(*this);
    return old;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator--()
  {
    p -= 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  basic_uint16_iterator<T>::operator--(int x)
  {
    basic_uint16_iterator<T> old = *this;
    --(*this);
    return old;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator+=(ptrdiff_t n)
  {
    p += n * 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T> &
  basic_uint16_iterator<T>::operator-=(ptrdiff_t n)
  {
    p -= n * 2;
    return *this;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  operator+(const basic_uint16_iterator<T> &x, ptrdiff_t n)
  {
    basic_uint16_iterator<T> tmp(x);
    return tmp += n;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  operator+(ptrdiff_t n, const basic_uint16_iterator<T> &x)
  {
    return x + n;
  }

  template <class T>
  inline basic_uint16_iterator<T>
  operator-(const basic_uint16_iterator<T> &x, ptrdiff_t n)
  {
    basic_uint16_iterator<T> tmp(x);
    return tmp -= n;
  }

  template <class T>
  inline ptrdiff_t
  operator-(const basic_uint16_iterator<T> &x,
	    const basic_uint16_iterator<T> &y)
  {
    return (x.base() - y.base()) / 2;
  }

  template <class T>
  inline bool
  operator<(const basic_uint16_iterator<T> &x,
	    const basic_uint16_iterator<T> &y)
  {
    return x.base() < y.base();
  }

  template <class T>
  inline uint16_type
  basic_uint16_iterator<T>::operator[](ptrdiff_t n) const
  {
    return *(*this + n);
  }

  template <class T>
  inline basic_uint16_iterator<T>::ref
  basic_uint16_iterator<T>::operator[](ptrdiff_t n)
  {
    return *(*this + n);
  }

  template <class T>
  inline
  basic_uint16_iterator<T>::basic_uint16_iterator(const T &ptr)
    : p(ptr)
  {
  }

  template <class T>
  template <class U>
  inline
  basic_uint16_iterator<T>::basic_uint16_iterator(const
						  basic_uint16_iterator<U> &x)
    : p(x.base())
  {
  }
  
  /* Implementation of `class basic_uint32_iterator'.  */

  template <class T>
  inline basic_uint32_iterator<T>::ref &
  basic_uint32_iterator<T>::ref::operator=(const uint32_type &value)
  {
    p[0] = value >> 24;
    p[1] = value >> 16;
    p[2] = value >>  8;
    p[3] = value;
    return *this;
  }

  template <class T>
  inline
  basic_uint32_iterator<T>::ref::operator uint32_type() const
  {
    return (uint32_type(p[0] & 0xff) << 24 | uint32_type(p[1] & 0xff) << 16
	    | uint32_type(p[2] & 0xff) << 8 | uint32_type(p[3] & 0xff));
  }

  template <class T>
  inline
  basic_uint32_iterator<T>::ref::ref(const T &ptr)
    : p(ptr)
  {
  }

  template <class T>
  inline bool
  operator==(const basic_uint32_iterator<T> &x,
	     const basic_uint32_iterator<T> &y)
  {
    return x.base() == y.base();
  }

  template <class T>
  inline bool
  operator!=(const basic_uint32_iterator<T> &x,
	     const basic_uint32_iterator<T> &y)
  {
    return !(x == y);
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator++()
  {
    p += 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  basic_uint32_iterator<T>::operator++(int x)
  {
    basic_uint32_iterator<T> old = *this;
    ++(*this);
    return old;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator--()
  {
    p -= 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  basic_uint32_iterator<T>::operator--(int x)
  {
    basic_uint32_iterator<T> old = *this;
    --(*this);
    return old;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator+=(ptrdiff_t n)
  {
    p += n * 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T> &
  basic_uint32_iterator<T>::operator-=(ptrdiff_t n)
  {
    p -= n * 4;
    return *this;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  operator+(const basic_uint32_iterator<T> &x, ptrdiff_t n)
  {
    basic_uint32_iterator<T> tmp(x);
    return tmp += n;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  operator+(ptrdiff_t n, const basic_uint32_iterator<T> &x)
  {
    return x + n;
  }

  template <class T>
  inline basic_uint32_iterator<T>
  operator-(const basic_uint32_iterator<T> &x, ptrdiff_t n)
  {
    basic_uint32_iterator<T> tmp(x);
    return tmp -= n;
  }

  template <class T>
  inline ptrdiff_t
  operator-(const basic_uint32_iterator<T> &x,
	    const basic_uint32_iterator<T> &y)
  {
    return (x.base() - y.base()) / 2;
  }

  template <class T>
  inline bool
  operator<(const basic_uint32_iterator<T> &x,
	    const basic_uint32_iterator<T> &y)
  {
    return x.base() < y.base();
  }

  template <class T>
  inline uint32_type
  basic_uint32_iterator<T>::operator[](ptrdiff_t n) const
  {
    return *(*this + n);
  }

  template <class T>
  inline basic_uint32_iterator<T>::ref
  basic_uint32_iterator<T>::operator[](ptrdiff_t n)
  {
    return *(*this + n);
  }

  template <class T>
  inline
  basic_uint32_iterator<T>::basic_uint32_iterator(const T &ptr)
    : p(ptr)
  {
  }

  template <class T>
  template <class U>
  inline
  basic_uint32_iterator<T>::basic_uint32_iterator(const
						  basic_uint32_iterator<U> &x)
    : p(x.base())
  {
  }
}

#endif /* not __VM68K_ITERATOR_H */
