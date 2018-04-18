lib_LIBRARIES     = libhiscore.a 
libhiscore_a_SOURCES = hiscore.c
INCLUDES=-I../src
if HSNET
libhiscore_a_SOURCES+=hsnet.c
AM_CPPFLAGS=$(LIBCURL_CPPFLAGS)
endif
