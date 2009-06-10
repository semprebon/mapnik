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
//$Id: graphics.cpp 17 2005-03-08 23:58:43Z pavlenko $

// mapnik
#include <mapnik/graphics.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/image_reader.hpp>

// cairo
#ifdef HAVE_CAIRO
// required for rendering vector symbols to contexts
#include <mapnik/cairo_renderer.hpp>

#include <cairomm/surface.h>

// librsvg
#ifdef HAVE_RSVG
extern "C"
{
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
}
#endif // HAVE_RSVG
#endif

namespace mapnik
{
#ifdef HAVE_CAIRO
#ifdef HAVE_RSVG
    SvgSymbol::SvgSymbol(int width,int height)
        :width_(width), height_(height), xscale_(1.0), yscale_(1.0), hRsvg_(0)
        {}
    SvgSymbol::SvgSymbol(int width,int height, double xscale, double yscale)
        :width_(width), height_(height), xscale_(xscale), yscale_(yscale), hRsvg_(0)
        {}
    SvgSymbol::SvgSymbol()
        :width_(0), height_(0), xscale_(1.0), yscale_(1.0), hRsvg_(0)
        {}
    SvgSymbol::SvgSymbol(const SvgSymbol& rhs)
        :width_(rhs.width_),
         height_(rhs.height_),
         xscale_(rhs.xscale_),
         yscale_(rhs.yscale_),
         hRsvg_(0) {}
         
    void SvgSymbol::load_from_file(std::string filename, int width, int height)
    {
        GError *error = 0;
        RsvgHandle *h = rsvg_handle_new_from_file(filename.c_str(), &error);
        if (error || !h) throw ImageReaderException("cannot open image file "+filename);
        hRsvg_ = h;
    }
    
    void SvgSymbol::render_to_context(Cairo::RefPtr<Cairo::Context>& context, double x, double y, double opacity) const
    {
        
        /* this metasurface caching not only makes things faster, it is required for 
         * cairo to recognize and re-use symbols when rendering vector output.
         */
        if (!surface_meta_)
        {
            Cairo::RefPtr<Cairo::Surface> surface = context->get_target();
            
            // could use surface->get_content() here instead but it requires cairomm 1.8
            cairo_content_t c_content = cairo_surface_get_content(const_cast<cairo_surface_t*>(surface->cobj()));
            Cairo::Content content = static_cast<Cairo::Content>(c_content);
            
            surface_meta_ = Cairo::Surface::create(surface, content, width_ * xscale_, height_ * yscale_);
            
            Cairo::RefPtr<Cairo::Context> c_meta = Cairo::Context::create(surface_meta_);
            c_meta->scale(xscale_, yscale_);
            c_meta->save();
            rsvg_handle_render_cairo(hRsvg_, c_meta->cobj());
            c_meta->restore();
        }

        context->save();
        Cairo::RefPtr<Cairo::SurfacePattern> pattern = Cairo::SurfacePattern::create(surface_meta_);
        
        Cairo::Matrix matrix;
        pattern->get_matrix(matrix);
        matrix.x0 = -x;
        matrix.y0 = -y;
        pattern->set_matrix(matrix);
        
        context->set_source(pattern);
        context->paint_with_alpha(opacity);
        context->restore();
    }
    
    const boost::shared_ptr<const Image32> SvgSymbol::rasterize() const
    {
        Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width_*xscale_, height_*yscale_);
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);

        context->save();
        context->scale(xscale_, yscale_);
        rsvg_handle_render_cairo(hRsvg_, context->cobj());
        context->restore();
        
        return boost::shared_ptr<const Image32>(new Image32(surface));
    }
#endif
#endif

    Image32::Image32(int width,int height, double xscale, double yscale)
        :width_(width),
         height_(height),
         xscale_(xscale),
         yscale_(yscale),
         data_(width,height)
#ifdef HAVE_CAIRO
         , surface_meta_(0)
#endif
     {}
         
    Image32::Image32(int width,int height)
        :width_(width),
         height_(height),
         xscale_(1.0),
         yscale_(1.0),
         data_(width,height)
#ifdef HAVE_CAIRO
         , surface_meta_(0)
#endif
     {}

    Image32::Image32(const Image32& rhs)
        :width_(rhs.width_),
         height_(rhs.height_),
         xscale_(rhs.xscale_),
         yscale_(rhs.yscale_),
         data_(rhs.data_)
#ifdef HAVE_CAIRO
         , surface_meta_(0)
#endif
     {}

#ifdef HAVE_CAIRO
    Image32::Image32(Cairo::RefPtr<Cairo::ImageSurface> rhs, double xscale, double yscale)
        :width_(rhs->get_width()),
         height_(rhs->get_height()),
         xscale_(xscale),
         yscale_(yscale),
         data_(rhs->get_width(),rhs->get_height()),
         surface_meta_(0)
        {
            if (rhs->get_format() != Cairo::FORMAT_ARGB32)
            {
                    std::cerr << "Unable to convert this Cairo format\n";
                    return; // throw exception ??
            }

            int stride = rhs->get_stride() / 4;

            unsigned int out_row[width_];
            const unsigned int *in_row = (const unsigned int *)rhs->get_data();

            for (unsigned int row = 0; row < height_; row++, in_row += stride)
            {
                for (unsigned int column = 0; column < width_; column++)
                {
                   unsigned int in = in_row[column];
                   unsigned int a = (in >> 24) & 0xff;
                   unsigned int r = (in >> 16) & 0xff;
                   unsigned int g = (in >> 8) & 0xff;
                   unsigned int b = (in >> 0) & 0xff;

    #define DE_ALPHA(x) do { \
                       if (a == 0) x = 0; \
                       else x = x * 255 / a; \
                       if (x > 255) x = 255; \
                   } while(0)

                   DE_ALPHA(r);
                   DE_ALPHA(g);
                   DE_ALPHA(b);

                   out_row[column] = color(r, g, b, a).rgba();
                }
                data_.setRow(row, out_row, width_);
            }
        }
#endif

    Image32::~Image32() {}

    const ImageData32& Image32::data() const
    {
        return data_;
    }
    
    const boost::shared_ptr<const Image32> Image32::rasterize() const
    {
        if (xscale_ == 1.0 && yscale_ == 1.0)
            return shared_from_this();
        
        Image32 *scaled = new Image32(xscale_*width_, yscale_*height_);
        scale_image_bilinear(scaled->data(), data_);
        return boost::shared_ptr<const Image32>(scaled);
    }

    void Image32::setBackground(const color& background)
    {
        background_=background;
        data_.set(background_.rgba());
    }

    const color& Image32::getBackground() const
    {
        return background_;
    }

#ifdef HAVE_CAIRO
    void Image32::render_to_context(Cairo::RefPtr<Cairo::Context>& context, double x, double y, double opacity) const
    {
        /* this metasurface caching not only makes things faster, it is required for 
         * cairo to recognize and re-use symbols when rendering vector output.
         */
        if (!surface_meta_)
        {
            Cairo::RefPtr<Cairo::Surface> surface = context->get_target();
            
            // could use surface->get_content() here instead but it requires cairomm 1.8
            cairo_content_t c_content = cairo_surface_get_content(const_cast<cairo_surface_t*>(surface->cobj()));
            Cairo::Content content = static_cast<Cairo::Content>(c_content);
            
            surface_meta_ = Cairo::Surface::create(surface, content, width_ * xscale_, height_ * yscale_);
            cairo_pattern pattern(data_);
            Cairo::RefPtr<Cairo::Pattern> p_meta = pattern.pattern();
            
            Cairo::RefPtr<Cairo::Context> c_meta = Cairo::Context::create(surface_meta_);
            c_meta->scale(xscale_, yscale_);
            c_meta->save();
            c_meta->set_source(p_meta);
            c_meta->paint_with_alpha(opacity);
            c_meta->restore();
        }

        context->save();
        Cairo::RefPtr<Cairo::SurfacePattern> pattern = Cairo::SurfacePattern::create(surface_meta_);
        
        Cairo::Matrix matrix;
        pattern->get_matrix(matrix);
        matrix.x0 = -x;
        matrix.y0 = -y;
        pattern->set_matrix(matrix);
        
        context->set_source(pattern);
        context->paint_with_alpha(1.0);
        context->restore();
    }
#endif
}
