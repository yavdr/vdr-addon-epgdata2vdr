/*
 * channelmap.h: TVTV plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _CHANNELMAP__H
#define _CHANNELMAP__H

#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <vdr/plugin.h>
#include <vdr/tools.h>

using namespace std;

typedef vector<char *> cChanVec;

typedef map<int, cChanVec> cChanMap;

typedef map<int, bool> cVPSMap;

class cChannelMap {
private:
  void remove_whitespaces(char *s);
  int read_config_file();
public:
  cChannelMap();
  ~cChannelMap();
  cChanMap chanmap;
  cVPSMap vpsmap;
  int ReloadChannelMap();
  int GetChanCnt(int tvmid);
  char *GetChanStr(int tvmid, int index);
//  tChannelID GetChanID(int tvmid, int index);
};

#endif
