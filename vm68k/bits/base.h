/* -*- C++ -*- */
/* Base definitions for Virtual M68000 Toolkit
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

#ifndef _VM68K_BASE_H
#define _VM68K_BASE_H 1

#include <climits>

#ifndef VM68K_PUBLIC
#define VM68K_PUBLIC
#endif

namespace vx68k_m68k
{
#if SCHAR_MIN < -0x7f
  typedef signed char data8_t;
#else
  typedef short       data8_t;
#endif
#if SHRT_MIN < -0x7fff
  typedef short       data16_t;
#elif INT_MIN < -0x7fff
  typedef int         data16_t;
#else
  typedef long        data16_t;
#endif
#if INT_MIN < -0x7fffffffL && INT_MAX >= 0x7fffffffL
  typedef int         data32_t;
#elif LONG_MIN < -0x7fffffffL
  typedef long        data32_t;
#else
#error No suitable type for data32_t
#endif

  typedef unsigned char  udata8_t;
  typedef unsigned short udata16_t;
#if UINT_MAX >= 0xffffffffUL
  typedef unsigned int   udata32_t;
#else
  typedef unsigned long  udata32_t;
#endif

  typedef int      data_fast8_t;
  typedef int      data_fast16_t;
  typedef data32_t data_fast32_t;

  typedef unsigned int udata_fast8_t;
  typedef unsigned int udata_fast16_t;
  typedef udata32_t    udata_fast32_t;

  typedef udata_fast32_t address_t;
}

#endif
