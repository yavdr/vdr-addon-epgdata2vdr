/*
 * update.h: epgdata plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _UPDATE__H
#define _UPDATE__H

#include <cstdlib>
#include <string>
#include <ctime>

#include <zip.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "channelmap.h"
#include "datamap.h"

typedef struct {
// mapping of external data
	char *name;
	cChannelMap *chanmap;
	cDataMap *datamap;
	int chanindex;
// data
	int broadcast_id;
	int tvshow_id;
	int regional ;
	int tvchannel_id;
	time_t starttime;
	time_t vps;
	int tvshow_length;
	string primetime ; 		
	string category ;
	string genre;
	xmlChar *technics_bw;
	xmlChar *technics_co_channel;
	xmlChar *technics_vt150;
	xmlChar *technics_coded;
	xmlChar *technics_blind;
	xmlChar *age_marker;
	xmlChar *live;
	xmlChar *tip;
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
} UserData, * UserDataPtr;
using namespace std; 

class cProcessEpg {
private:
	void processNode(xmlTextReaderPtr reader, UserDataPtr &user_data);
public:
	cProcessEpg();
	~cProcessEpg();
	UserDataPtr user_data;
	int processFile(UserDataPtr user_data, char *filename);
};

#endif
