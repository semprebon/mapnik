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

#ifndef FILTER_FACTORY_HPP
#define FILTER_FACTORY_HPP

#include <mapnik/config_error.hpp>
#include <mapnik/filter_parser.hpp>

namespace mapnik
{
    using std::string;
    
    template<typename FeatureT>
    class MAPNIK_DECL filter_factory
    {
    public:
        static filter_ptr compile(string const& str)
        {
            stack<shared_ptr<filter<FeatureT> > > filters;
            stack<shared_ptr<expression<FeatureT> > > exps;
            filter_grammar<FeatureT> grammar(filters,exps);
            char const *text = str.c_str();
            parse_info<> info = parse(text, grammar, space_p);
            if ( ! info.full) {
                std::ostringstream os;
                os << "Failed to parse filter expression:" << std::endl
                   << str << std::endl
                   << "Parsing aborted at '" << info.stop << "'";

                throw config_error( os.str() );
            }

            if ( ! filters.empty())
            {
                return filters.top();	
            }
            else 
            {
                // XXX: do we ever get here? [DS]
                return filter_ptr(new none_filter<FeatureT>());
            }  
        }
    };
    
    MAPNIK_DECL filter_ptr create_filter (std::string const& wkt);
}

#endif //FILTER_FACTORY_HPP
