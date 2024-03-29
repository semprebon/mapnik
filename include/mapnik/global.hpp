/*****************************************************************************
 * 
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2006 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

//$Id$

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <boost/cstdint.hpp>


namespace mapnik
{
 
#define int2net(A)  (int16_t) (((boost::uint16_t) ((boost::uint8_t) (A)[1]))      | \
                               (((boost::uint16_t) ((boost::uint8_t) (A)[0])) << 8))

#define int4net(A)  (int32_t) (((boost::uint32_t) ((boost::uint8_t) (A)[3]))      | \
                               (((boost::uint32_t) ((boost::uint8_t) (A)[2])) << 8)  | \
                               (((boost::uint32_t) ((boost::uint8_t) (A)[1])) << 16) | \
                               (((boost::uint32_t) ((boost::uint8_t) (A)[0])) << 24))


   typedef boost::uint8_t byte;
#define float8net(V,M)   do { double def_temp;  \
    ((byte*) &def_temp)[0]=(M)[7];		\
    ((byte*) &def_temp)[1]=(M)[6];		\
    ((byte*) &def_temp)[2]=(M)[5];		\
    ((byte*) &def_temp)[3]=(M)[4];		\
    ((byte*) &def_temp)[4]=(M)[3];		\
    ((byte*) &def_temp)[5]=(M)[2];		\
    ((byte*) &def_temp)[6]=(M)[1];		\
    ((byte*) &def_temp)[7]=(M)[0];		\
    (V) = def_temp; } while(0)
#define float4net(V,M)   do { float def_temp;   \
    ((byte*) &def_temp)[0]=(M)[3];		\
    ((byte*) &def_temp)[1]=(M)[2];		\
    ((byte*) &def_temp)[2]=(M)[1];		\
    ((byte*) &def_temp)[3]=(M)[0];		\
    (V)=def_temp; } while(0)
}

#endif //GLOBAL_HPP
