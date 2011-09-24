/*
 * channelmap.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "channelmap.h"


cChannelMap::cChannelMap (string conffile)
{
	chanmap.clear ();
	read_config_file(conffile);
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
cChannelMap::read_config_file (string mapfile)
{
	ifstream cmfile;
	string s;
	size_t p;
	int tvmid;
	int n;

	cmfile.open (mapfile.c_str());
	if (!cmfile)
	{
	    fprintf(stderr,"Unable to open channelmap file: %s.\n", mapfile.c_str());
		return -1;
	}

	fprintf(stderr,"Load '%s': ", mapfile.c_str());
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
	fprintf(stderr,"%d channel mappings read.\n", n);
	return n;
}

cChanMap::iterator chaniter;

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


void
cChannelMap::GetAllChanStr()
{
    for(chaniter = cChannelMap::chanmap.begin(); chaniter != cChannelMap::chanmap.end(); chaniter++) {
       for (unsigned i=0; i < chaniter->second.size(); i++) {
         printf("%s ",chaniter->second[i]); 
       }

    }
}
