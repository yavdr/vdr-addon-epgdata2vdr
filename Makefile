
all:	test

test: main2.c update.h update.c channelmap.h channelmap.c datamap.c
	$(CXX) -Wall -g -I/usr/include/libxml2 -I/usr/include/libzip main2.c update.h update.c channelmap.h channelmap.c datamap.c -lzip -lxml2 -o test