/*
 * datamap.h: TVTV plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _DATAMAP__H
#define _DATAMAP__H

#include <map>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>


typedef std::map<int, char *> cDMap;

class cDataMap {
private:
  int read_xml_file();
  void processData(xmlTextReaderPtr reader);
public:
  cDataMap();
  ~cDataMap();
  cDMap datamap;
  char* GetStr(int dataid);
};

#endif
