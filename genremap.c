/*
 * genremap.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "genremap.h"


cGenreMap::cGenreMap (string conffile)
{
	genremap.clear ();
	read_config_file(conffile);
}

cGenreMap::~cGenreMap ()
{
	genremap.clear ();
}


void
cGenreMap::remove_whitespaces (char *s)
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
cGenreMap::read_config_file (string mapfile)
{
	ifstream cmfile;
	string s;
	size_t p;
	int tvmid;
	int n;

	cmfile.open (mapfile.c_str());
	if (!cmfile)
	{
	    fprintf(stderr,"Unable to open genremap file: %s.\n", mapfile.c_str());
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
					genremap[tvmid].push_back(ptr);

					// are there more genreids separated by commas?
		        	        while ((ptr = index(ptr, ',')) != NULL)
					{
						*ptr++ = '\0';
						genremap[tvmid].push_back(ptr);
					}
				}
				n++;
			}
		}
	}
	cmfile.close ();
	fprintf(stderr,"%d genre mappings read.\n", n);
	return n;
}


int
cGenreMap::GetGenreCount(int genreid)
{
	return genremap[genreid].size();
}

char *
cGenreMap::GetGenreString (int genreid, int index)
{
	return genremap[genreid][index];
}

