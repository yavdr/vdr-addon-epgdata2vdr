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

using namespace std; 

typedef map<int, string> cDMap;

class cDataMap {
private:
  int read_xml_file();
  void processData(xmlTextReaderPtr reader);
public:
  cDataMap();
  ~cDataMap();
  cDMap datamap;
  string GetStr(int dataid);
};

#endif
