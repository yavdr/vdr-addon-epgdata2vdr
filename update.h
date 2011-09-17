/*
 * update.h: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _UPDATE__H
#define _UPDATE__H

#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include <set>
#include <algorithm>
#include <clocale>
#include <langinfo.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef USE_IMAGEMAGICK
#include <magick/api.h>
#ifdef VERSION // Workaround for old and buggy imagemagick
#undef VERSION
#endif
#endif

#include <zip.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "channelmap.h"
#include "datamap.h"

typedef struct {
// mapping of external data
	char *name;
	int chanindex;
        int item;

// data
	// coming from map
	string category;
	string genre;

	//coming from XML
	int regional;
	int tvchannel_id;
	time_t starttime;
	time_t vps;
	int tvshow_length;
	xmlChar *broadcast_id;
	xmlChar *tvshow_id;
	xmlChar *primetime ;
	xmlChar *technics_bw;
	xmlChar *technics_co_channel;
	xmlChar *technics_vt150;
	xmlChar *technics_coded;
	xmlChar *technics_blind;
	xmlChar *age_marker;
	xmlChar *live;
	int 	tip;
	xmlChar *title;
	xmlChar *subtitle;
	xmlChar *comment_long;
	xmlChar *comment_middle;
	xmlChar *comment_short;
	xmlChar *themes;
	xmlChar *sequence;
	xmlChar *technics_stereo;
	xmlChar *technics_dolby;
	xmlChar *technics_wide;
	xmlChar *stars;
	xmlChar *attribute;
	xmlChar *country;
	xmlChar *moderator;
	xmlChar *year;
	xmlChar *studio_guest;
	xmlChar *regisseur;
	xmlChar *actor;
	string  sourcepic ;
} UserData, * UserDataPtr;
using namespace std;


class cProcessEpg {
private:
	void processNode(xmlTextReaderPtr reader, xmlTextWriterPtr writer, UserDataPtr &user_data);
        std::set< int > importedItems;
        int imageCount;
public:
	cProcessEpg();
	~cProcessEpg();
	cChannelMap *chanmap;
	cDataMap *datamap;
	int processFile(string confdir, char *filename);
    void readMaps() ;
	string epgimagesdir ;     /* -i option */
    string procdir;           /* -p option */
    int    imgsize;           /* -s option */
    string outfmt;            /* -o option */
    string incdir;            /* -I option */
    string channelmapfile;    /* -c option */
    string imageformat;       /* -f option */
};

#endif
