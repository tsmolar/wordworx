#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT(ww, 1.9.0, asmolar@sf.net)
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
#AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_RANLIB
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_ARG_ENABLE(widescreen,
[  --enable-widescreen  use widescreen mode by default (needs at least 800x480 display)
],[AC_DEFINE([WIDESCREEN],[1],[Define if using Widescreen (by default)])
   use_widescreen=yes])
#AC_ARG_ENABLE(zaurus,
#[  --enable-zaurus  Enable Optimizations for Zaurus and other small devices
#],[CFLAGS="$CFLAGS -DZAURUS"])

# Why this mess?   If ww is in the PATH, then it won't be able to
# locate the datadir, so INSTPREFIX will be where to look
if test "x$use_zaurus" = "xyes" ; then
  AC_DEFINE_UNQUOTED([INSTPREFIX],["/home/QtPalmtop"],[Help find data dir])
elif test "x$prefix" = "xNONE" ; then
    AC_DEFINE_UNQUOTED([INSTPREFIX],["/usr/local"],[Help find data dir])
else
    AC_DEFINE_UNQUOTED([INSTPREFIX],"${prefix}",[Help find data dir])
fi

# Checks for libraries.
CHECK_ZDLL()
if test "$have_zdll" = "yes"
then
    AM_CONDITIONAL(USEZ,true)
    LDFLAGS="$LDFLAGS $ZDLL_LDFLAGS"
    CPPFLAGS="$CPPFLAGS $ZDLL_CPPFLAGS"
else
  CHECK_ZLIB()
  if test "$have_zlib" = "yes"
  then
     AM_CONDITIONAL(USEZ,true)
  else
     AM_CONDITIONAL(USEZ,false)
  fi
fi

LIBCURL_CHECK_CONFIG([no], [7.13.2], [haveCurl=yes], [haveCurl=no])
if test "x$haveCurl" = "xyes"
then
  AM_CONDITIONAL(HSNET,true)
  LIBS="$LIBS $LIBCURL"
 rem LIBS="$LIBS -L/opt/libcurl-7.16.3/lib -lcurl"
else
  AM_CONDITIONAL(HSNET,false)
fi

# Freetype
AC_CHECK_FT2([2.1.0], [haveFT=yes], [haveFT=no])
if test "$haveFT" = "yes"
then
  AM_CONDITIONAL(FREETYPE,true)
  AC_DEFINE([USE_FREETYPE],[1],[Define if using FreeType])
  CFLAGS="$CFLAGS $FT2_CFLAGS"
  LIBS="$LIBS $FT2_LIBS"
else
  AM_CONDITIONAL(FREETYPE,false)
fi

gfx_lib=null
dnl Want SDL2 or SDL
dnl Check for SDL2
AM_CONDITIONAL(USESDL2,false)
AM_CONDITIONAL(USESDL,false)

AC_MSG_NOTICE([SDL2 check])
AM_PATH_SDL2(2.0.0,gfx_lib=sdl2,:)
if [[ "$gfx_lib" = sdl2 ]]; then
  AC_MSG_NOTICE([Choosing SDL2 Library])
  AM_CONDITIONAL(USESDL2,true)
  CFLAGS="$CFLAGS $SDL_CFLAGS"
  LIBS="$LIBS $SDL_LIBS"
  AC_CHECK_LIB(SDL2_image,IMG_Load)
  AC_CHECK_LIB(SDL2_mixer,Mix_Linked_Version)
else
  AC_MSG_NOTICE([SDL1 check])
  AM_PATH_SDL(1.2.5,gfx_lib=sdl1,:)
  if [[ "$gfx_lib" = sdl1 ]]; then
    AC_MSG_NOTICE([Choosing SDL1 Library])
    AM_CONDITIONAL(USESDL,true)
    CFLAGS="$CFLAGS $SDL_CFLAGS"
    LIBS="$LIBS $SDL_LIBS"
    AC_CHECK_LIB(SDL_image,IMG_Load)
    AC_CHECK_LIB(SDL_mixer,Mix_Linked_Version)
  else
    if [[ "$gfx_lib" = null ]]; then
      AC_MSG_ERROR([* * * No compatible graphics library found!])
    fi
  fi
fi

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 widget/Makefile
		 hiscore/Makefile
		 sdlallegro/Makefile])
AC_OUTPUT
