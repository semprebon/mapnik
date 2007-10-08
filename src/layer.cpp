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

//$Id: layer.cpp 17 2005-03-08 23:58:43Z pavlenko $

// mapnik
#include <mapnik/layer.hpp>

#include <mapnik/style.hpp>
#include <mapnik/datasource.hpp>
#include <mapnik/datasource_cache.hpp>
// boost
#include <boost/shared_ptr.hpp>
// stl
#include <string>
#include <iostream>

using namespace std;
using boost::shared_ptr;

namespace mapnik
{   
    Layer::Layer(std::string const& name, std::string const& srs)
        : name_(name),
          title_(""),
          abstract_(""),
          srs_(srs),
          minZoom_(0),
          maxZoom_(std::numeric_limits<double>::max()),
          active_(true),
          queryable_(false),
          clear_label_cache_(false),
          ds_() {}
    
    Layer::Layer(const Layer& rhs)
        : name_(rhs.name_),
          title_(rhs.title_),
          abstract_(rhs.abstract_),
          srs_(rhs.srs_),
          minZoom_(rhs.minZoom_),
          maxZoom_(rhs.maxZoom_),
          active_(rhs.active_),
          queryable_(rhs.queryable_),
          clear_label_cache_(rhs.clear_label_cache_),
          styles_(rhs.styles_),
          ds_(rhs.ds_) {}
    
    Layer& Layer::operator=(const Layer& rhs)
    {
        Layer tmp(rhs);
        swap(tmp);
        return *this;
    }

    bool Layer::operator==(Layer const& other) const
    {
        return (this == &other);
    }
    
    void Layer::swap(const Layer& rhs)
    {
        name_=rhs.name_;
        title_=rhs.title_;
        abstract_=rhs.abstract_;
        minZoom_=rhs.minZoom_;
        maxZoom_=rhs.maxZoom_;
        active_=rhs.active_;
        queryable_=rhs.queryable_;
        styles_=rhs.styles_;
        ds_=rhs.ds_;
    }
    
    Layer::~Layer() {}
    
    void Layer::set_name( std::string const& name)
    {
        name_ = name;
    }
 
    string const& Layer::name() const
    {
        return name_;
    }

    void Layer::set_title( std::string const& title)
    {
        title_ = title;
    }
 
    string const& Layer::title() const
    {
        return title_;
    }
    
    void Layer::set_abstract( std::string const& abstract)
    {
        abstract_ = abstract;
    }
 
    string const& Layer::abstract() const
    {
        return abstract_;
    }

    void Layer::set_srs(std::string const& srs)
    {
        srs_ = srs;
    }
    
    std::string const& Layer::srs() const
    {
        return srs_;
    }
    
    void Layer::add_style(std::string const& stylename)
    {
        styles_.push_back(stylename);
    }
    
    std::vector<std::string> const& Layer::styles() const
    {
        return styles_;
    }
    
    std::vector<std::string> & Layer::styles()
    {
        return styles_;
    }

    void Layer::setMinZoom(double minZoom)
    {
        minZoom_=minZoom;
    }

    void Layer::setMaxZoom(double maxZoom)
    {
        maxZoom_=maxZoom;
    }

    double Layer::getMinZoom() const
    {
        return minZoom_;
    }

    double Layer::getMaxZoom() const
    {
        return maxZoom_;
    }

    void Layer::setActive(bool active)
    {
        active_=active;
    }

    bool Layer::isActive() const
    {
        return active_;
    }

    bool Layer::isVisible(double scale) const
    {
        return isActive() && scale >= minZoom_ - 1e-6 && scale < maxZoom_ + 1e-6;
    }

    void Layer::setQueryable(bool queryable)
    {
        queryable_=queryable;
    }

    bool Layer::isQueryable() const
    {
        return queryable_;
    }

    datasource_ptr Layer::datasource() const
    {
        return ds_;
    }
    
    void Layer::set_datasource(datasource_ptr const& ds)
    {
        ds_ = ds;
    }
    
    Envelope<double> Layer::envelope() const
    {
        if (ds_) return ds_->envelope();
    	return Envelope<double>();
    }
    
   void Layer::set_clear_label_cache(bool clear)
   {
      clear_label_cache_ = clear;
   }
   
   bool Layer::clear_label_cache() const
   {
      return clear_label_cache_;
   }
}
