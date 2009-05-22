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

#ifdef HAVE_CAIRO
#ifdef HAVE_RSVG

#include <mapnik/image_reader.hpp>

extern "C"
{
#include <librsvg/rsvg.h>
}

#include <iostream>

namespace mapnik
{
    class SvgReader : public ImageReader 
    {
    private:
        std::string fileName_;
        unsigned width_;
        unsigned height_;
    public:
        explicit SvgReader(const std::string& fileName);
        ~SvgReader();
        unsigned width() const;
        unsigned height() const;
        boost::shared_ptr<ISymbol> init_symbol(double xscale, double yscale) const;
        void read(unsigned x,unsigned y,ISymbol& image);
    private:
        SvgReader(const SvgReader&);	
        SvgReader& operator=(const SvgReader&);
        void init();
    };

    namespace
    {
        ImageReader* createSvgReader(const std::string& file)
        {
            return new SvgReader(file);
        }
        const bool registered = register_image_reader("svg",createSvgReader);
    }

    SvgReader::SvgReader(const std::string& fileName) 
        : fileName_(fileName),
          width_(0),
          height_(0)
    {
        init();
    }

    SvgReader::~SvgReader() {}
  

    void SvgReader::init()
    {
        rsvg_init();
        GError *error = 0;
        RsvgHandle *svg = rsvg_handle_new_from_file(fileName_.c_str(), &error);
        
        if (error || !svg) throw ImageReaderException("cannot open image file "+fileName_);
        
        RsvgDimensionData dimensions;
        rsvg_handle_get_dimensions (svg, &dimensions);
        
        width_ = dimensions.width;
        height_ = dimensions.height;
        
        rsvg_handle_free(svg);
    }

    unsigned SvgReader::width() const 
    {
        return width_;
    }

    unsigned SvgReader::height() const 
    {
        return height_;
    }
    
    boost::shared_ptr<ISymbol> SvgReader::init_symbol(double xscale, double yscale) const
    {
        return boost::shared_ptr<ISymbol>(new SvgSymbol(width_, height_, xscale, yscale));
    }
    
    void SvgReader::read(unsigned x0, unsigned y0,ISymbol& symbol)
    {
        SvgSymbol& svgSymbol = static_cast<SvgSymbol&>(symbol);
        
        svgSymbol.load_from_file(fileName_, width_, height_);
    }
}
#endif // HAVE_RSVG
#endif // HAVE_CAIRO
