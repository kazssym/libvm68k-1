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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307
   USA.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if __GNUC__
#define VM68K_PUBLIC __attribute__ ((__visibility__ ("default")))
#endif

#include <vm68k/size>

namespace vx68k_m68k
{
  const byte_size      byte =      {};
  const word_size      word =      {};
  const long_word_size long_word = {};
}
