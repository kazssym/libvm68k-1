/* -*-c++-*-
 * base - base unit private header for Virtual M68000 Toolkit
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

#ifndef _VM68K_BASE_H
#define _VM68K_BASE_H 1

#include <climits>

#ifndef VM68K_PUBLIC
#if _WIN32
#define VM68K_PUBLIC __declspec (dllimport)
#else
#define VM68K_PUBLIC
#endif
#endif

#if __GNUC__
#define _VM68K_DEPRECATED __attribute__ ((deprecated))
#else
#define _VM68K_DEPRECATED
#endif

namespace vx68k
{
#if SCHAR_MIN < -0x7f
  typedef signed char int_least8_t;
#else
  typedef short       int_least8_t;
#endif
#if SHRT_MIN < -0x7fff
  typedef short       int_least16_t;
#elif INT_MIN < -0x7fff
  typedef int         int_least16_t;
#else
  typedef long        int_least16_t;
#endif
#if INT_MIN < -0x7fffffffL && INT_MAX >= 0x7fffffffL
  typedef int         int_least32_t;
#elif LONG_MIN < -0x7fffffffL
  typedef long        int_least32_t;
#else
#error No suitable type for int_least32_t
#endif

  typedef unsigned char  uint_least8_t;
  typedef unsigned short uint_least16_t;
#if UINT_MAX >= 0xffffffffUL
  typedef unsigned int   uint_least32_t;
#else
  typedef unsigned long  uint_least32_t;
#endif

  typedef int            int_fast8_t;
  typedef int            int_fast16_t;
  typedef int_least32_t  int_fast32_t;

  typedef unsigned int   uint_fast8_t;
  typedef unsigned int   uint_fast16_t;
  typedef uint_least32_t uint_fast32_t;
}

#endif
