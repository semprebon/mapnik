if HAVE_BOOST_PROGRAM_OPTIONS

bin_PROGRAMS = \
	ogrindex

ogrindex_SOURCES = \
	quadtree.hpp\
	ogrindex.cpp

ogrindex_LDFLAGS = \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	../../src/libmapnik.la \
	${AGG_LIBS}

ogrindex_DEPENDENCIES = \
	../../src/libmapnik.la

ogrindex_CXXFLAGS = \
        ${PROFILING_CFLAGS} \
        ${TRACING_CFLAGS} \
	-I../../include \
	-I../../plugins/input/ogr \
	${AGG_CFLAGS}

endif

## File created by the gnome-build tools

