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
AM_CFLAGS=-DUSESDL -DUSEGFX -g
ww_LDADD=../widget/libwidget.a ../widget/libfont.a ../sdlallegro/libsdlallegro.a ../hiscore/libhiscore.a 
INCLUDES = -I../sdlallegro -I../widget
endif

if USESDL2
ww_SOURCES+=gfx.c
AM_CFLAGS=-DUSESDL -DUSEGFX -g
ww_LDADD=../widget/libwidget.a ../widget/libfont.a ../sdlallegro/libsdlallegro.a ../hiscore/libhiscore.a 
INCLUDES = -I../sdlallegro -I../widget
endif
