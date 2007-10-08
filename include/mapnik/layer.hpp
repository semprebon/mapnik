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
//$Id: layer.hpp 39 2005-04-10 20:39:53Z pavlenko $

#ifndef LAYER_HPP
#define LAYER_HPP
// mapnik
#include <mapnik/feature.hpp>
#include <mapnik/datasource.hpp>
// boost
#include <boost/shared_ptr.hpp>
// stl
#include <vector>

namespace mapnik
{
    class MAPNIK_DECL Layer
    {
    public:
        explicit Layer(std::string const& name, std::string const& srs="+proj=latlong +datum=WGS84");
        Layer(Layer const& l);
        Layer& operator=(Layer const& l);
        bool operator==(Layer const& other) const;
        void set_name(std::string const& name);
        const std::string& name() const;
        void set_title(std::string const& title);
        const std::string& title() const;
        void set_abstract(std::string const& abstract);
        const std::string& abstract() const;
        void set_srs(std::string const& srs);
        std::string const& srs() const;
        void add_style(std::string const& stylename);
        std::vector<std::string> const& styles() const;
        std::vector<std::string>& styles();
        void setMinZoom(double minZoom);
        void setMaxZoom(double maxZoom);
        double getMinZoom() const;
        double getMaxZoom() const;
        void setActive(bool active);
        bool isActive() const;
        void setQueryable(bool queryable);
        bool isQueryable() const;
        bool isVisible(double scale) const;
        void set_clear_label_cache(bool clear_cache);
        bool clear_label_cache() const; 
        void set_datasource(datasource_ptr const& ds);
        datasource_ptr datasource() const;
        Envelope<double> envelope() const;
        ~Layer();
    private:
        void swap(const Layer& other);

        std::string name_;
        std::string title_;
        std::string abstract_;
        std::string srs_;
        
        double minZoom_;
        double maxZoom_;
        bool active_;
        bool queryable_;
        bool clear_label_cache_;
        std::vector<std::string>  styles_;
        datasource_ptr ds_;
    };
}

#endif //LAYER_HPP
