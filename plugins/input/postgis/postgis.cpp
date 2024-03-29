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

//$Id: postgis.cc 44 2005-04-22 18:53:54Z pavlenko $

// mapnik
#include "connection_manager.hpp"
#include "postgis.hpp"
#include <mapnik/ptree_helpers.hpp>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

// stl
#include <string>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>


DATASOURCE_PLUGIN(postgis_datasource)

const std::string postgis_datasource::GEOMETRY_COLUMNS="geometry_columns";
const std::string postgis_datasource::SPATIAL_REF_SYS="spatial_ref_system";

using std::clog;
using std::endl;

using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::shared_ptr;

using mapnik::PoolGuard;
using mapnik::attribute_descriptor;

postgis_datasource::postgis_datasource(parameters const& params)
   : datasource (params),
     table_(*params.get<std::string>("table","")),
     geometry_field_(*params.get<std::string>("geometry_field","")),
     cursor_fetch_size_(*params_.get<int>("cursor_size",0)),
     row_limit_(*params_.get<int>("row_limit",0)),
     type_(datasource::Vector),
     extent_initialized_(false),
     desc_(*params.get<std::string>("type"),"utf-8"),
     creator_(params.get<std::string>("host"),
              params.get<std::string>("port"),
              params.get<std::string>("dbname"),
              params.get<std::string>("user"),
              params.get<std::string>("password"))    
{   

   boost::optional<int> initial_size = params_.get<int>("inital_size",1);
   boost::optional<int> max_size = params_.get<int>("max_size",10);

   multiple_geometries_ = *params_.get<mapnik::boolean>("multiple_geometries",false);
   
   boost::optional<std::string> ext  = params_.get<std::string>("extent");
   if (ext)
   {
      boost::char_separator<char> sep(",");
      boost::tokenizer<boost::char_separator<char> > tok(*ext,sep);
      unsigned i = 0;
      bool success = false;
      double d[4];
      for (boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); 
           beg!=tok.end();++beg)
      {
         try 
         {
            d[i] = boost::lexical_cast<double>(boost::trim_copy(*beg));
         }
         catch (boost::bad_lexical_cast & ex)
         {
            std::clog << *beg << " : " << ex.what() << "\n";
            break;
         }
         if (i==3) 
         {
            success = true;
            break;
         }
         ++i;
      }
      if (success)
      {
         extent_.init(d[0],d[1],d[2],d[3]);
         extent_initialized_ = true;
      }
   } 

   ConnectionManager *mgr=ConnectionManager::instance();   
   mgr->registerPool(creator_, *initial_size, *max_size);
    
   shared_ptr<Pool<Connection,ConnectionCreator> > pool=mgr->getPool(creator_.id());
   if (pool)
   {      
      shared_ptr<Connection> conn = pool->borrowObject();
      if (conn && conn->isOK())
      {
         PoolGuard<shared_ptr<Connection>,
            shared_ptr<Pool<Connection,ConnectionCreator> > > guard(conn,pool);
         
         desc_.set_encoding(conn->client_encoding());
         
         std::string table_name=table_from_sql(table_);
         std::string schema_name="";
         std::string::size_type idx=table_name.find_last_of('.');
         if (idx!=std::string::npos)
         {
            schema_name=table_name.substr(0,idx);
            table_name=table_name.substr(idx+1);
         }
         else
         {
            table_name=table_name.substr(0);
         }

         std::ostringstream s;
         s << "select f_geometry_column,srid,type from ";
         s << GEOMETRY_COLUMNS <<" where f_table_name='" << table_name<<"'";
         
         if (schema_name.length() > 0)
            s <<" and f_table_schema='"<< schema_name <<"'";

         if (geometry_field_.length() > 0)
            s << " and f_geometry_column = '" << geometry_field_ << "'";
            
         shared_ptr<ResultSet> rs=conn->executeQuery(s.str());
         if (rs->next())
         {
            try 
            {
               srid_ = lexical_cast<int>(rs->getValue("srid"));
            }
            catch (bad_lexical_cast &ex)
            {
               clog << rs->getValue("srid") << ":" << ex.what() << endl;
            }
            geometryColumn_=rs->getValue("f_geometry_column");
            std::string postgisType=rs->getValue("type");
         }
         rs->close();
            
         // collect attribute desc
         s.str("");
         s << "select * from " << table_ << " limit 0";
         rs=conn->executeQuery(s.str());
         int count = rs->getNumFields();
         for (int i=0;i<count;++i)
         {
            std::string fld_name=rs->getFieldName(i);
            int type_oid = rs->getTypeOID(i);
            switch (type_oid)
            {
               case 21:    // int2
               case 23:    // int4
                  desc_.add_descriptor(attribute_descriptor(fld_name,mapnik::Integer));
                  break;
               case 700:   // float4 
               case 701:   // float8
               case 1700:  // numeric ??
                  desc_.add_descriptor(attribute_descriptor(fld_name,mapnik::Double));
               case 1042:  // bpchar
               case 1043:  // varchar
               case 25:    // text
                  desc_.add_descriptor(attribute_descriptor(fld_name,mapnik::String));
                  break;
               default: // shouldn't get here
#ifdef MAPNIK_DEBUG
                  clog << "unknown type_oid="<<type_oid<<endl;
#endif
                  break;
            }	  
         }
      }
   }
}

std::string const postgis_datasource::name_="postgis";

std::string postgis_datasource::name()
{
   return name_;
}

int postgis_datasource::type() const
{
   return type_;
}

layer_descriptor postgis_datasource::get_descriptor() const
{
   return desc_;
}

std::string postgis_datasource::table_from_sql(const std::string& sql)
{
   std::string table_name = boost::algorithm::to_lower_copy(sql);
   boost::algorithm::replace_all(table_name,"\n"," ");
   
   std::string::size_type idx = table_name.rfind(" from ");
   if (idx!=std::string::npos)
   {
      
      idx=table_name.find_first_not_of(" ",idx+5);
      if (idx != std::string::npos)
      {
         table_name=table_name.substr(idx);
      }
      idx=table_name.find_first_of(" )");
      if (idx != std::string::npos)
      {
         table_name = table_name.substr(0,idx);
      }
   }
   return table_name;
}

boost::shared_ptr<IResultSet> postgis_datasource::get_resultset(boost::shared_ptr<Connection> const &conn, const std::string &sql) const
{
     if (cursor_fetch_size_ > 0) {
         // cursor
         std::ostringstream csql;
         std::string cursor_name = conn->new_cursor_name();

         csql << "DECLARE " << cursor_name << " BINARY INSENSITIVE NO SCROLL CURSOR WITH HOLD FOR " << sql << " FOR READ ONLY";

#ifdef MAPNIK_DEBUG
         std::clog << csql.str() << "\n";
#endif
         if (!conn->execute(csql.str())) {
            throw mapnik::datasource_exception( "PSQL Error: Creating cursor for data select." );
         }
         return shared_ptr<CursorResultSet>(new CursorResultSet(conn, cursor_name, cursor_fetch_size_));

     } else {
         // no cursor
#ifdef MAPNIK_DEBUG
         std::clog << sql << "\n";
#endif
         return conn->executeQuery(sql,1);
     }
}

featureset_ptr postgis_datasource::features(const query& q) const
{
   Envelope<double> const& box=q.get_bbox();
   ConnectionManager *mgr=ConnectionManager::instance();
   shared_ptr<Pool<Connection,ConnectionCreator> > pool=mgr->getPool(creator_.id());
   if (pool)
   {
      shared_ptr<Connection> conn = pool->borrowObject();
      if (conn && conn->isOK())
      {       
         PoolGuard<shared_ptr<Connection>,shared_ptr<Pool<Connection,ConnectionCreator> > > guard(conn,pool);
         std::ostringstream s;
            
         s << "SELECT AsBinary(\""<<geometryColumn_<<"\") AS geom";
         std::set<std::string> const& props=q.property_names();
         std::set<std::string>::const_iterator pos=props.begin();
         std::set<std::string>::const_iterator end=props.end();
         while (pos != end)
         {
            s <<",\""<<*pos<<"\"";
            ++pos;
         }	 
         s << " from " << table_ << " WHERE \""<<geometryColumn_<<"\" && SetSRID('BOX3D(";
         s << std::setprecision(16);
         s << box.minx() << " " << box.miny() << ",";
         s << box.maxx() << " " << box.maxy() << ")'::box3d,"<<srid_<<")";
         
         if (row_limit_ > 0) {
             s << " LIMIT " << row_limit_;
         }
         
         boost::shared_ptr<IResultSet> rs = get_resultset(conn, s.str());
         return featureset_ptr(new postgis_featureset(rs,desc_.get_encoding(),multiple_geometries_,props.size()));
      }
   }
   return featureset_ptr();
}

featureset_ptr postgis_datasource::features_at_point(coord2d const& pt) const
{
   ConnectionManager *mgr=ConnectionManager::instance();
   shared_ptr<Pool<Connection,ConnectionCreator> > pool=mgr->getPool(creator_.id());
   if (pool)
   {
      shared_ptr<Connection> conn = pool->borrowObject();
      if (conn && conn->isOK())
      {       
         PoolGuard<shared_ptr<Connection>,shared_ptr<Pool<Connection,ConnectionCreator> > > guard(conn,pool);
         std::ostringstream s;
            
         s << "SELECT AsBinary(\"" << geometryColumn_ << "\") AS geom";
            
         std::vector<attribute_descriptor>::const_iterator itr = desc_.get_descriptors().begin();
         std::vector<attribute_descriptor>::const_iterator end = desc_.get_descriptors().end();
         unsigned size=0;
         while (itr != end)
         {
            s <<",\""<< itr->get_name() << "\"";
            ++itr;
            ++size;
         }
            
         s << " FROM " << table_ << " WHERE \""<<geometryColumn_<<"\" && SetSRID('BOX3D(";
         s << std::setprecision(16);
         s << pt.x << " " << pt.y << ",";
         s << pt.x << " " << pt.y << ")'::box3d,"<<srid_<<")";
         
         if (row_limit_ > 0) {
             s << " LIMIT " << row_limit_;
         }
         
         boost::shared_ptr<IResultSet> rs = get_resultset(conn, s.str());
         return featureset_ptr(new postgis_featureset(rs,desc_.get_encoding(),multiple_geometries_, size));
      }
   }
   return featureset_ptr();
}

Envelope<double> postgis_datasource::envelope() const
{
   if (extent_initialized_) return extent_;
    
   ConnectionManager *mgr=ConnectionManager::instance();
   shared_ptr<Pool<Connection,ConnectionCreator> > pool=mgr->getPool(creator_.id());
   if (pool)
   {
      shared_ptr<Connection> conn = pool->borrowObject();
      if (conn && conn->isOK())
      {
         PoolGuard<shared_ptr<Connection>,shared_ptr<Pool<Connection,ConnectionCreator> > > guard(conn,pool);
         std::ostringstream s;
         std::string table_name = table_from_sql(table_);
         boost::optional<std::string> estimate_extent = params_.get<std::string>("estimate_extent");
         
         if (estimate_extent && *estimate_extent == "true")
         {
            s << "select xmin(ext),ymin(ext),xmax(ext),ymax(ext)"
              << " from (select estimated_extent('" 
              << table_name <<"','" 
              << geometryColumn_ << "') as ext) as tmp";
         }
         else 
         {
            s << "select xmin(ext),ymin(ext),xmax(ext),ymax(ext)"
              << " from (select extent(" <<geometryColumn_<< ") as ext from " 
              << table_name << ") as tmp";
         }
            
         shared_ptr<ResultSet> rs=conn->executeQuery(s.str());
         if (rs->next())
         {
            try 
            {
               double lox=lexical_cast<double>(rs->getValue(0));
               double loy=lexical_cast<double>(rs->getValue(1));
               double hix=lexical_cast<double>(rs->getValue(2));
               double hiy=lexical_cast<double>(rs->getValue(3));		    
               extent_.init(lox,loy,hix,hiy);
               extent_initialized_ = true;
            }
            catch (bad_lexical_cast &ex)
            {
               clog << ex.what() << endl;
            }
         }
         rs->close();
      }
   }
   return extent_;
}

postgis_datasource::~postgis_datasource() {}
