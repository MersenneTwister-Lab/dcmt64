noinst_PROGRAMS = dcmt64 calc_equidist

dcmt64_SOURCES = dcmt64.cpp mt64Search.hpp mpicontrol.hpp search.h \
search.cpp best_search.cpp MixedSequence.hpp options.h options.cpp

calc_equidist_SOURCES = mt64Search.hpp calc_equidist.cpp

AM_CXXFLAGS = -Wall -O2 -Wextra -D__STDC_CONSTANT_MACROS \
              -D__STDC_FORMAT_MACROS
EXTRA_DIST = Makefile.mpi dcmt64mpi.cpp
