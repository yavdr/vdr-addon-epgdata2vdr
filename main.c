
#include <string>

#include "update.h" 
using namespace std;


int main(int argc, char *argv[])
{
	int n;
	cProcessEpg *process ; 
	process = new cProcessEpg();
	process->confdir = string(argv[1]) ;
#ifdef USE_IMAGEMAGICK
	process->epgimagesdir = string(argv[2]) ;
#endif
	process->readMaps(process->confdir) ; 

#ifndef USE_IMAGEMAGICK	
	for (n=2; n<argc; n++)
#else
	for (n=3; n<argc; n++)
#endif
	{
		process->processFile(process->confdir, argv[n]);
	}
	delete process;
	return 0 ;
}
