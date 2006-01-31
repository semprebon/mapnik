/* This file is part of Mapnik (c++ mapping toolkit)
 * Copyright (C) 2005 Artem Pavlenko
 *
 * Mapnik is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//$Id: raster_info.hh 17 2005-03-08 23:58:43Z pavlenko $

#ifndef RASTER_INFO
#define RASTER_INFO

#include "raster_datasource.hpp"
#include <string>

using mapnik::Envelope;

class raster_info
{
    std::string file_;
    std::string format_;
    Envelope<double> extent_;
    int srid_;
    public:
    raster_info(const std::string& file,const std::string& format,const Envelope<double>& extent,int srid=-1);
    raster_info(const raster_info& rhs);
    raster_info& operator=(const raster_info& rhs);
    const Envelope<double>& envelope() const;
    const std::string& file() const;
    const std::string& format() const;
    const int srid() const;
private:
    void swap(raster_info& other) throw();
};
#endif                                            //RASTER_INFO
