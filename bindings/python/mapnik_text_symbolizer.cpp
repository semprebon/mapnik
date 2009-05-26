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
//$Id$

#include <boost/python.hpp>

#include <mapnik/text_symbolizer.hpp>
#include "mapnik_enumeration.hpp"

using namespace mapnik;
using mapnik::color;
using mapnik::text_symbolizer;

namespace {
  using namespace boost::python;

  list get_displacement_list(const text_symbolizer& t)
  {
    position pos = t.get_displacement();
    double dx = boost::get<0>(pos);
    double dy = boost::get<1>(pos);
    boost::python::list disp;
    disp.append(dx);
    disp.append(dy);
    return disp;
  }
  
  list get_anchor_list(const text_symbolizer& t)
  {
    position anch = t.get_anchor();
    double x = boost::get<0>(anch);
    double y = boost::get<1>(anch);
    boost::python::list anchor;
    anchor.append(x);
    anchor.append(y);
    return anchor;
  }
}

struct text_symbolizer_pickle_suite : boost::python::pickle_suite
{
   static boost::python::tuple
   getinitargs(const text_symbolizer& t)
   {

      return boost::python::make_tuple(t.get_name(),t.get_face_name(),t.get_text_size(),t.get_fill());
      
   }

   static  boost::python::tuple
   getstate(const text_symbolizer& t)
   {
        boost::python::list disp = get_displacement_list(t);
        boost::python::list anchor = get_anchor_list(t);
                
        return boost::python::make_tuple(disp,t.get_fontset().get_name(),t.get_label_placement(),
        t.get_vertical_alignment(),t.get_halo_radius(),t.get_halo_fill(),t.get_text_ratio(),
        t.get_wrap_width(),t.get_label_spacing(),t.get_minimum_distance(),t.get_allow_overlap(),
        anchor,t.get_force_odd_labels(),t.get_max_char_angle_delta());
   }

   static void
   setstate (text_symbolizer& t, boost::python::tuple state)
   {
        using namespace boost::python;
        
        if (len(state) != 14)
        {
            PyErr_SetObject(PyExc_ValueError,
                         ("expected 14-item tuple in call to __setstate__; got %s"
                          % state).ptr()
            );
            throw_error_already_set();
        }
        
        list disp = extract<list>(state[0]);
        double dx = extract<double>(disp[0]);
        double dy = extract<double>(disp[1]);
        t.set_displacement(dx,dy);

        if (state[1])
        {
            std::string fontset = extract<std::string>(state[1]);
            t.set_fontset(fontset);
        }
        
        t.set_label_placement(extract<label_placement_e>(state[2]));

        t.set_vertical_alignment(extract<vertical_alignment_e>(state[3]));
        
        t.set_halo_radius(extract<unsigned>(state[4]));

        t.set_halo_fill(extract<color>(state[5]));

        t.set_text_ratio(extract<unsigned>(state[6]));

        t.set_wrap_width(extract<unsigned>(state[7]));

        t.set_label_spacing(extract<unsigned>(state[8]));

        t.set_minimum_distance(extract<double>(state[9]));

        t.set_allow_overlap(extract<bool>(state[10]));
        
        list anch = extract<list>(state[11]);
        double x = extract<double>(anch[0]);
        double y = extract<double>(anch[1]);
        t.set_anchor(x,y);
        
        t.set_force_odd_labels(extract<bool>(state[12]));
        
        t.set_max_char_angle_delta(extract<double>(state[13]));
   }

};


void export_text_symbolizer()
{
   using namespace boost::python;
 
   enumeration_<label_placement_e>("label_placement")
      .value("LINE_PLACEMENT",LINE_PLACEMENT)
      .value("POINT_PLACEMENT",POINT_PLACEMENT)
      ;
   enumeration_<vertical_alignment_e>("vertical_alignment")
      .value("TOP",TOP)
      .value("MIDDLE",MIDDLE)
      .value("BOTTOM",BOTTOM)
      ;
   
   class_<text_symbolizer>("TextSymbolizer",
                           init<std::string const&,std::string const&, unsigned,color const&>())
      .def_pickle(text_symbolizer_pickle_suite())
      .add_property("halo_fill",make_function(
                       &text_symbolizer::get_halo_fill,
                       return_value_policy<copy_const_reference>()),
                    &text_symbolizer::set_halo_fill)
      .add_property("halo_radius",
                    &text_symbolizer::get_halo_radius, 
                    &text_symbolizer::set_halo_radius)
      .add_property("wrap_width",
                    &text_symbolizer::get_wrap_width,
                    &text_symbolizer::set_wrap_width)
      .add_property("text_ratio",
                    &text_symbolizer::get_text_ratio,
                    &text_symbolizer::set_text_ratio)
      .add_property("label_spacing",
                    &text_symbolizer::get_label_spacing,
                    &text_symbolizer::set_label_spacing)
      .add_property("label_position_tolerance",
                    &text_symbolizer::get_label_position_tolerance,
                    &text_symbolizer::set_label_position_tolerance)
      .add_property("force_odd_labels",
                    &text_symbolizer::get_force_odd_labels,
                    &text_symbolizer::set_force_odd_labels)

      .add_property("fontset",
                    make_function(&text_symbolizer::get_fontset,return_value_policy<copy_const_reference>()),
                    &text_symbolizer::set_fontset)

      .add_property("fill",              
                    make_function(&text_symbolizer::get_fill,return_value_policy<copy_const_reference>()),
                    &text_symbolizer::set_fill)
      .add_property("name",
                    make_function(&text_symbolizer::get_name,return_value_policy<copy_const_reference>()),
                    &text_symbolizer::set_name)

      .add_property("text_size",
                    &text_symbolizer::get_text_size,
                    &text_symbolizer::set_text_size)

      .add_property("face_name",
                    make_function(&text_symbolizer::get_face_name,return_value_policy<copy_const_reference>()),
                    &text_symbolizer::set_face_name)
                    
      .add_property("max_char_angle_delta",
                    &text_symbolizer::get_max_char_angle_delta,
                    &text_symbolizer::set_max_char_angle_delta)
      .add_property("avoid_edges",
                    &text_symbolizer::get_avoid_edges,
                    &text_symbolizer::set_avoid_edges)
      .add_property("minimum_distance",
                    &text_symbolizer::get_minimum_distance,
                    &text_symbolizer::set_minimum_distance)
      .def("displacement",&text_symbolizer::set_displacement)
      .def("anchor",&text_symbolizer::set_anchor)
      .def("get_displacement",get_displacement_list)
      .def("get_anchor",get_anchor_list)
      .add_property("label_placement",
                    &text_symbolizer::get_label_placement,
                    &text_symbolizer::set_label_placement,
                    "Set/get the placement of the label")

      .add_property("vertical_alignment",
                    &text_symbolizer::get_vertical_alignment,
                    &text_symbolizer::set_vertical_alignment,
                    "Set/get the vertical alignment of the label")
      
      .add_property("allow_overlap",
                    &text_symbolizer::get_allow_overlap,
                    &text_symbolizer::set_allow_overlap,
                    "Set/get the allow_overlap property of the label")
      ;
}
