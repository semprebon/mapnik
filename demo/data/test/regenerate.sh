#!/bin/sh
rm -f textspacing.shp textspacing.shx textspacing.dbf
ogr2ogr -f "ESRI Shapefile" textspacing raw/textspacing.gml
mv textspacing/* ./
rmdir textspacing

rm -f overlap.shp overlap.shx overlap.dbf
ogr2ogr -f "ESRI Shapefile" overlap raw/overlap.gml
mv overlap/* ./
rmdir overlap