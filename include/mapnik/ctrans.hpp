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

//$Id: ctrans.hpp 39 2005-04-10 20:39:53Z pavlenko $

#ifndef CTRANS_HPP
#define CTRANS_HPP

#include <mapnik/envelope.hpp>
#include <mapnik/coord_array.hpp>
#include <mapnik/proj_transform.hpp>

namespace mapnik {
   typedef coord_array<coord2d> CoordinateArray;
    
   template <typename Transform,typename Geometry>
   struct MAPNIK_DECL coord_transform
   {
      coord_transform(Transform const& t, Geometry& geom)
         : t_(t), geom_(geom) {}
        
      unsigned  vertex(double *x , double *y) const
      {
         unsigned command = geom_.vertex(x,y);
         t_.forward(x,y);
         return command;
      }
        
      void rewind (unsigned pos)
      {
         geom_.rewind(pos);
      }
        
     private:
      Transform const& t_;
      Geometry& geom_;
   };

   template <typename Transform,typename Geometry>
   struct MAPNIK_DECL coord_transform2
   {
      coord_transform2(Transform const& t, 
                       Geometry const& geom, 
                       proj_transform const& prj_trans)
         : t_(t), 
         geom_(geom), 
         prj_trans_(prj_trans)  {}
        
      unsigned  vertex(double * x , double  * y) const
      {
         unsigned command = geom_.vertex(x,y);
         double z=0;
         prj_trans_.backward(*x,*y,z);
         t_.forward(x,y);
         return command;
      }
        
      void rewind (unsigned pos)
      {
         geom_.rewind(pos);
      }
        
     private:
      Transform const& t_;
      Geometry const& geom_;
      proj_transform const& prj_trans_;
   };
    
   template <typename Transform,typename Geometry>
   struct MAPNIK_DECL coord_transform3
   {
      coord_transform3(Transform const& t, 
                       Geometry const& geom, 
                       proj_transform const& prj_trans,
                       int dx, int dy)
         : t_(t), 
         geom_(geom), 
         prj_trans_(prj_trans),
         dx_(dx), dy_(dy) {}
      
      unsigned  vertex(double * x , double  * y) const
      {
         unsigned command = geom_.vertex(x,y);
         double z=0;
         prj_trans_.backward(*x,*y,z);
         t_.forward(x,y);
         *x+=dx_;
         *y+=dy_;
         return command;
      }
      
      void rewind (unsigned pos)
      {
         geom_.rewind(pos);
      }
      
     private:
      Transform const& t_;
      Geometry const& geom_;
      proj_transform const& prj_trans_;
      int dx_;
      int dy_;
   };
   
   class CoordTransform
   {
      private:
         int width;
         int height;
         double scale_;
         Envelope<double> extent_;
         double offset_x_;
         double offset_y_;
      public:
         CoordTransform(int width,int height,const Envelope<double>& extent,
                        double offset_x = 0, double offset_y = 0)
            :width(width),height(height),extent_(extent),offset_x_(offset_x),offset_y_(offset_y)
         {
            double sx=((double)width)/extent_.width();
            double sy=((double)height)/extent_.height();
            scale_=std::min(sx,sy);
         }
	
         inline double scale() const
         {
            return scale_;
         }
         
         inline void forward(double * x, double * y) const
         {
            *x = (*x - extent_.minx()) * scale_ - offset_x_;
            *y = (extent_.maxy() - *y) * scale_ - offset_y_;
         }
        
         inline void backward(double * x, double * y) const
         {
            *x = extent_.minx() + (*x + offset_x_)/scale_;
            *y = extent_.maxy() - (*y + offset_y_)/scale_;
         }
         
         inline coord2d& forward(coord2d& c) const
         {
            forward(&c.x,&c.y);
            return c;
         }
         
         inline coord2d& backward(coord2d& c) const
         {
            backward(&c.x,&c.y);
            return c;
         }

         inline Envelope<double> forward(const Envelope<double>& e) const
         {
            double x0 = e.minx();
            double y0 = e.miny();
            double x1 = e.maxx();
            double y1 = e.maxy();
            forward(&x0,&y0);
            forward(&x1,&y1);
            return Envelope<double>(x0,y0,x1,y1);
         }

         inline Envelope<double> backward(const Envelope<double>& e) const
         {
            double x0 = e.minx();
            double y0 = e.miny();
            double x1 = e.maxx();
            double y1 = e.maxy();
            backward(&x0,&y0);
            backward(&x1,&y1);
            return Envelope<double>(x0,y0,x1,y1);
         }

         inline CoordinateArray& forward(CoordinateArray& coords) const
         {
            for (unsigned i=0;i<coords.size();++i)
            {
               forward(coords[i]);
            }
            return coords;
         }
	
         inline CoordinateArray& backward(CoordinateArray& coords) const
         {
            for (unsigned i=0;i<coords.size();++i)
            {
               backward(coords[i]);
            }
            return coords;
         }
         inline Envelope<double> const& extent() const
         {
            return extent_;
         }
   };
}

#endif //CTRANS_HPP
