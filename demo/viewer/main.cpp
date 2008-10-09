/* This file is part of Mapnik (c++ mapping toolkit)
 * Copyright (C) 2006 Artem Pavlenko
 *
 * Mapnik is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//$Id$


#include <QApplication>
#include <QStringList>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include "mainwindow.hpp"

int main( int argc, char **argv )
{
   using mapnik::datasource_cache;
   using mapnik::freetype_engine;
  
   datasource_cache::instance()->register_datasources("/opt/mapnik/lib/mapnik/input");
   
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSans-Bold.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSans-BoldOblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSans-ExtraLight.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSans-Oblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSans.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansCondensed-Bold.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansCondensed-BoldOblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansCondensed-Oblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansCondensed.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansMono-Bold.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansMono-BoldOblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansMono-Oblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSansMono.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerif-Bold.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerif-BoldOblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerif-Oblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerif.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerifCondensed-Bold.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerifCondensed-BoldOblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerifCondensed-Oblique.ttf");
   freetype_engine::register_font("/opt/mapnik/lib/mapnik/fonts/DejaVuSerifCondensed.ttf");
   
   QApplication app( argc, argv ); 
   MainWindow window;
   window.show();
   if (argc > 1) window.open(argv[1]);
   if (argc == 3)
   {
      QStringList list = QString(argv[2]).split(",");
      if (list.size()==4)
      {
         bool ok;
         double x0 = list[0].toDouble(&ok);
         double y0 = list[1].toDouble(&ok);
         double x1 = list[2].toDouble(&ok);
         double y1 = list[3].toDouble(&ok);
         if (ok) window.set_default_extent(x0,y0,x1,y1); 
      }
   }
   
   return app.exec(); 
}
