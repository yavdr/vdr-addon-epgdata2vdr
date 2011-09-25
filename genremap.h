/*
 * channelmap.h: TVTV plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _GENREMAP__H
#define _GENREMAP__H

#include <map>
#include <vector>
#include <fstream>
#include <string>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef vector<char *> cGenreVec;
typedef map<int, cGenreVec> cGenMap;

class cGenreMap {
private:
  void remove_whitespaces(char *s);
  int read_config_file(string confdir);
public:
  cGenreMap(string confir);
  ~cGenreMap();
  cGenMap genremap;
  int GetGenreCount(int tvmid);
  char *GetGenreString(int tvmid, int index);
};

#endif
