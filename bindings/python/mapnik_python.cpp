/*****************************************************************************
 * 
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2006 Artem Pavlenko, Jean-Francois Doyon
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
//$Id: mapnik_python.cc 27 2005-03-30 21:45:40Z pavlenko $

#include <boost/python.hpp>
#include <boost/get_pointer.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <boost/python/exception_translator.hpp>

void export_color();
void export_coord();
void export_layer();
void export_parameters();
void export_envelope();
void export_query();
void export_image();
void export_image_view();
void export_map();
void export_python();
void export_filter();
void export_rule();
void export_style();
void export_stroke();
void export_feature();
void export_featureset();
void export_datasource();
void export_datasource_cache();
void export_point_symbolizer();
void export_line_symbolizer();
void export_line_pattern_symbolizer();
void export_polygon_symbolizer();
void export_polygon_pattern_symbolizer();
void export_raster_symbolizer();
void export_text_symbolizer();
void export_shield_symbolizer();
void export_font_engine();
void export_projection();

#include <mapnik/map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/graphics.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/config_error.hpp>
#include <mapnik/save_map.hpp>


void render(const mapnik::Map& map,mapnik::Image32& image, unsigned offset_x = 0, unsigned offset_y = 0)
{
    mapnik::agg_renderer<mapnik::Image32> ren(map,image,offset_x, offset_y);
    ren.apply();
}

void render2(const mapnik::Map& map,mapnik::Image32& image)
{
    mapnik::agg_renderer<mapnik::Image32> ren(map,image);
    ren.apply();
}

void render_tile_to_file(const mapnik::Map& map, 
                         unsigned offset_x, unsigned offset_y,
                         unsigned width, unsigned height,
                         const std::string& file,
                         const std::string& format)
{
    mapnik::Image32 image(width,height);
    render(map,image,offset_x, offset_y);
    mapnik::save_to_file(image.data(),file,format);
}

void render_to_file1(const mapnik::Map& map,
                    const std::string& filename,
                    const std::string& format)
{
   mapnik::Image32 image(map.getWidth(),map.getHeight());
   render(map,image,0,0);
   mapnik::save_to_file(image,filename,format); 
}

void render_to_file2(const mapnik::Map& map,
                    const std::string& filename)
{
   mapnik::Image32 image(map.getWidth(),map.getHeight());
   render(map,image,0,0);
   mapnik::save_to_file(image,filename); 
}


double scale_denominator(mapnik::Map const &map, bool geographic)
{
   return mapnik::scale_denominator(map, geographic);
}

void translator(mapnik::config_error const & ex) {
    PyErr_SetString(PyExc_UserWarning, ex.what());
}

BOOST_PYTHON_FUNCTION_OVERLOADS(load_map_overloads, load_map, 2, 3);

BOOST_PYTHON_MODULE(_mapnik)
{
 
    using namespace boost::python;

    using mapnik::load_map;
    using mapnik::save_map;

    register_exception_translator<mapnik::config_error>(translator);
    export_query();    
    export_feature();
    export_featureset();
    export_datasource();
    export_parameters();
    export_color(); 
    export_envelope();   
    export_image();
    export_image_view();
    export_filter();
    export_rule();
    export_style();    
    export_layer();
    export_stroke();
    export_datasource_cache();
    export_point_symbolizer();
    export_line_symbolizer();
    export_line_pattern_symbolizer();
    export_polygon_symbolizer();
    export_polygon_pattern_symbolizer();
    export_raster_symbolizer();
    export_text_symbolizer();
    export_shield_symbolizer();
    export_font_engine();
    export_projection();
    export_coord();
    export_map();
    
    def("render_to_file",&render_to_file1);
    def("render_to_file",&render_to_file2);
    def("render_tile_to_file",&render_tile_to_file);
    def("render",&render); 
    def("render",&render2);
    def("scale_denominator", &scale_denominator);
    
    def("load_map", & load_map, load_map_overloads());
    def("save_map", & save_map, "save Map object to XML");
    
    using mapnik::symbolizer;
    class_<symbolizer>("Symbolizer",no_init)
       ;
    
    register_ptr_to_python<mapnik::filter_ptr>();
}
