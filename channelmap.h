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

#include <cstdlib>
#include <cstring>

using namespace std;

typedef vector<char *> cChanVec;
typedef map<int, cChanVec> cChanMap;
typedef map<int, bool> cVPSMap;

class cChannelMap {
private:
  void remove_whitespaces(char *s);
  int read_config_file(string confdir);
public:
  cChannelMap(string confir);
  ~cChannelMap();
  cChanMap chanmap;
  cVPSMap vpsmap;
  int GetChanCnt(int tvmid);
  char *GetChanStr(int tvmid, int index);
  void GetAllChanStr();
//  tChannelID GetChanID(int tvmid, int index);
};

#endif
