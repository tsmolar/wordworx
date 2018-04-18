lib_LIBRARIES     = libfont.a libwidget.a
libfont_a_SOURCES = font.c
libwidget_a_SOURCES = button.c widget.c window.c keyboard.c input.c style.c
if USESDL
AM_CFLAGS=-DUSESDL -DCOMPFONT -DGFXLIB_ALLEGRO
INCLUDES=-I../sdlallegro
endif
if USEALLEGRO
AM_CFLAGS=-DCOMPFONT -DGFXLIB_ALLEGRO
endif
