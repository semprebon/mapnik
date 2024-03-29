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

#include <mapnik/symbolizer.hpp>

#include <mapnik/image_reader.hpp>

#include <iostream>

namespace mapnik {

   symbolizer_with_image::symbolizer_with_image(boost::shared_ptr<ISymbol> img) :
      image_( img ) {}
   
   symbolizer_with_image::symbolizer_with_image( symbolizer_with_image const& rhs)
      : image_(rhs.image_), image_filename_(rhs.image_filename_) {}

   symbolizer_with_image::symbolizer_with_image(std::string const& file,
                                                std::string const& type, unsigned width,unsigned height,
                                                float opacity)
      : image_filename_( file )
   {
      std::auto_ptr<ImageReader> reader(get_image_reader(file,type));
      if (reader.get()) 
      {
         double xscale = 1.0;
         double yscale = 1.0;
         if (width && !height) {
            xscale = yscale = width / (double)reader->width();
         } else if (height && !width) {
            xscale = yscale = height / (double)reader->height();
         } else if (height && width){
            xscale = width / (double)reader->width();
            yscale = height / (double)reader->height();
         }
         image_ = reader->init_symbol(xscale, yscale);
         reader->read(0,0,*image_);
      }
   }
   
   
   boost::shared_ptr<ISymbol> symbolizer_with_image::get_image() const
   {
      return image_;
   }
   void symbolizer_with_image::set_image(boost::shared_ptr<ISymbol> image) 
   {
      image_ = image;
   }
   
   std::string const& symbolizer_with_image::get_filename() const
   {
      return image_filename_;
   }

   void symbolizer_with_image::set_filename(std::string const& image_filename) 
   {
      image_filename_ = image_filename;
   }
      
} // end of namespace mapnik



