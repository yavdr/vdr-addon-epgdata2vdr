/*
 * channelmap.c: TVM2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "channelmap.h"


cChannelMap::cChannelMap (string confdir)
{
	chanmap.clear ();
	read_config_file(confdir);
}

cChannelMap::~cChannelMap ()
{
	chanmap.clear ();
}


void
cChannelMap::remove_whitespaces (char *s)
{
	unsigned int i = 0;
	char *dum;
	if (s != NULL)
	{
		dum = (char *) calloc (1, strlen (s) + 1);
		for (i = 0; i < strlen (s); i++)
			if (!isspace (s[i]))
				strncat (dum, &(s[i]), 1);
		memset (s, 0, strlen (s));
		strcpy (s, dum);
		free (dum);
	}
}


int
cChannelMap::read_config_file (string confdir)
{
	ifstream cmfile;
	string mapfile = confdir + "/epgdata2vdr_channelmap.conf" ;
	string s;
	size_t p;
	int tvmid;
	int n;
	char *cfg_fname = NULL;
	
	cmfile.open (mapfile.c_str());
	if (!cmfile)
	{
		//esyslog ("TVM2VDR: Error reading '%s'!", cfg_fname);
                fprintf(stderr,"TVM2VDR: Error reading '%s'!", cfg_fname);
		return -1;
	}
	//isyslog 
	//fprintf(stderr,"TVM2VDR: Loading '%s'", cfg_fname);
	n = 0;
	while (!cmfile.eof ())
	{
		getline (cmfile, s);

		if (!s.empty ())
		{
			remove_whitespaces ((char *) s.c_str ());

			// remove comments
			p = s.find_first_of ("//");
			if (p != string::npos)
				s.erase (p);

			// split line
			p = s.find_first_of ("=");
			if ((p != string::npos) && (s.substr (p + 1).length ()))
			{
				char *ptr = NULL;
				tvmid = atoi (s.substr (0, p).c_str ());
				if ((ptr = strdup(s.substr (p + 1).c_str ())) != NULL)
				{
					char *vpsptr = NULL;
					// one of the chars "1yYjJ" separated from the channelids
					// by a colon means the sender has VPS
					vpsmap[tvmid] = false;
					if ((vpsptr = index(ptr, ':')) != NULL)
					{
						*vpsptr++ = '\0';
						vpsmap[tvmid] = (index("1yYjJ", *vpsptr) != NULL);
					}
					chanmap[tvmid].push_back(ptr);
					// are there more channelids separated by commas?
		        	while ((ptr = index(ptr, ',')) != NULL)
					{
						*ptr++ = '\0';
						chanmap[tvmid].push_back(ptr);
					}
				}
				n++;
			}
		}
	}
	cmfile.close ();
	//isyslog ("TVM2VDR: %d channel mappings read.", n);
        //fprintf(stderr,"TVM2VDR: %d channel mappings read.", n);
	return n;
}

int
cChannelMap::GetChanCnt(int tvmid)
{
	return chanmap[tvmid].size();
}

char *
cChannelMap::GetChanStr (int tvmid, int index)
{
	return chanmap[tvmid][index];
}



