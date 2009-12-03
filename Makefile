
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

clean: 
	@rm -rf epgdata2vdr

install:
	install -m 750 epgdata2vdr $(DESTDIR)/usr/bin/
	install -m 750 epgdata2vdr.sh $(DESTDIR)/usr/bin/
	install -m 750 epgdata-update.cron-daily $(DESTDIR)/etc/cron.daily/epgdata2vdr-update
	install -d -m 750 $(DESTDIR)/var/cache/vdr/epgdata2vdr/include/
	install -d -m 750 $(DESTDIR)/var/cache/vdr/epgdata2vdr/files/
	install -m 660 epgdata2vdr_channelmap.conf $(DESTDIR)/var/cache/vdr/epgdata2vdr/include/
	install -d $(DESTDIR)/etc/vdr
	install -m 660 epgdata2vdr.conf $(DESTDIR)/etc/vdr/vdr-addon-epgdata2vdr.conf

uninstall: 
	@rm -rf $(DESTDIR)/var/cache/vdr/epgdata2vdr/include/
	@rm -rf $(DESTDIR)/var/cache/vdr/epgdata2vdr/files/
	@rm -f $(DESTDIR)/etc/cron.daily/epgdata2vdr-update
	@rm -f $(DESTDIR)/usr/bin/epgdata2vdr
	@rm -f $(DESTDIR)/usr/bin/epgdata2vdr.sh
	@rm -f $(DESTDIR)/var/cache/vdr/epgdata2vdr/include/epgdata2vdr_channelmap.conf
	@rm -f $(DESTDIR)/etc/vdr/vdr-addon-epgdata2vdr.conf
