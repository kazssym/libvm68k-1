/* -*-c++-*-
 * base - base unit private header for Virtual M68000 Toolkit
 * Copyright (C) 2003-2008 Hypercore Software Design, Ltd.
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

namespace vx68k
{
#if SCHAR_MIN < -0x7f
  typedef signed char vm68k_int8_t;
#else
  typedef short       vm68k_int8_t;
#endif
#if SHRT_MIN < -0x7fff
  typedef short       vm68k_int16_t;
#elif INT_MIN < -0x7fff
  typedef int         vm68k_int16_t;
#else
  typedef long        vm68k_int16_t;
#endif
#if INT_MIN < -0x7fffffffL && INT_MAX >= 0x7fffffffL
  typedef int         vm68k_int32_t;
#elif LONG_MIN < -0x7fffffffL
  typedef long        vm68k_int32_t;
#else
#error No suitable type for vm68k_int32_t
#endif

  typedef unsigned char  vm68k_uint8_t;
  typedef unsigned short vm68k_uint16_t;
#if UINT_MAX >= 0xffffffffUL
  typedef unsigned int   vm68k_uint32_t;
#else
  typedef unsigned long  vm68k_uint32_t;
#endif

  typedef int           vm68k_int_fast8_t;
  typedef int           vm68k_int_fast16_t;
  typedef vm68k_int32_t vm68k_int_fast32_t;

  typedef unsigned int   vm68k_uint_fast8_t;
  typedef unsigned int   vm68k_uint_fast16_t;
  typedef vm68k_uint32_t vm68k_uint_fast32_t;

  typedef vm68k_uint_fast32_t vm68k_address_t;
}

#endif
