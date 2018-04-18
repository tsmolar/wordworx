gfxdir=$(datadir)/wordworx/gfx
gfx_DATA=share/wordworx/gfx/*
snddir=$(datadir)/wordworx/sounds
snd_DATA=share/wordworx/sounds/*
hsdir=$(datadir)/wordworx/hs
hs_DATA=share/wordworx/hs/hsnet.txt
icondir=$(datadir)/wordworx/icons
icon_DATA=share/wordworx/icons/*

bin_PROGRAMS=ww
ww_SOURCES=main.c dict.c ai.c dfilepath.c

if USEZ
dictdir=$(datadir)/wordworx/zdict
dict_DATA=share/wordworx/zdict/*
else
dictdir=$(datadir)/wordworx/dict
dict_DATA=share/wordworx/dict/*
endif

if USESDL
ww_SOURCES+=gfx.c
AM_CFLAGS=-DUSESDL -DUSEGFX
ww_LDADD=../hiscore/libhiscore.a ../sdlallegro/libsdlallegro.a ../widget/libfont.a ../widget/libwidget.a
INCLUDES = -I../sdlallegro -I../widget
endif

if USEALLEGRO
ww_SOURCES+=gfx.c
AM_CFLAGS=-DUSEALLEGRO -DUSEGFX $(allegro_CFLAGS)
AM_CXXFLAGS=-DUSEALLEGRO -DUSEGFX $(allegro_CFLAGS)
ww_LDADD=../hiscore/libhiscore.a ../widget/libfont.a ../widget/libwidget.a $(allegro_LIBS)
INCLUDES = -I../widget
endif

