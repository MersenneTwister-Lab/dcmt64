#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dcmt64], [0.1], [sai10@hiroshima-u.ac.jp])
#LT_INIT
AC_CONFIG_SRCDIR([src/Makefile.am])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# Checks for programs.
: ${CFLAGS=""}
: ${CXXFLAGS=""}
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_LIBTOOL

# Checks for libraries.
AX_PTHREAD
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# Open MPI compiler
#AC_ARG_ENABLE([mpi],
#        AS_HELP_STRING([--enable-mpi], [make open mpi search executables]))
#AM_CONDITIONAL([MPI], [test x$enable_mpi = xyes])

#if test !AS_VAR_TEST_SET([MPICXX]); then
#  AS_VAR_SET([MPICXX], [`which mpicxx`])
#fi
#AC_SUBST(MPICXX)

# gcc arch flag
#AX_GCC_ARCHFLAG

# keep this order
AC_CHECK_LIB(gf2x, gf2x_mul)
AC_CHECK_LIB(gmp, __gmpz_init)
AC_CHECK_LIB(ntl, main,[],[ AC_MSG_ERROR(Need ntl) ])
AC_CHECK_LIB(MTToolBox, get_mttoolbox_version,[],[AC_MSG_ERROR(Need MTToolBox)])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h])
#AX_EXT

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_INT64_T
#AC_TYPE_UINT64_T

AC_LANG_PUSH(C++)
AX_CXX_COMPILE_STDCXX_0X

AX_CXX_COMPILE_STDCXX_17(noext, optional) # keep this order
AX_CXX_COMPILE_STDCXX_14(noext, optional)
AX_CXX_COMPILE_STDCXX_11(noext, optional)
AC_LANG_POP

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
