
USE_IMAGEMAGICK=1

ifdef USE_IMAGEMAGICK
LIBS += $(shell Magick-config --ldflags --libs)
endif

ifdef USE_IMAGEMAGICK
DEFINES = -DUSE_IMAGEMAGICK $(shell Magick-config --cflags --cppflags)
endif


all:	epgdata2vdr

epgdata2vdr: main.c update.h update.c channelmap.h channelmap.c datamap.c
	$(CXX) -Wall -g -I/usr/include/libxml2 -I/usr/include/libzip $(DEFINES) main.c update.h update.c channelmap.h channelmap.c datamap.c -lzip -lxml2 $(LIBS) -o epgdata2vdr
