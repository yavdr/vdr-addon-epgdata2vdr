#include <cstdlib>
#include <string>

#include "update.h" 
using namespace std;


int main(int argc, char *argv[])
{
	int n;
	cProcessEpg *process ; 
	process = new cProcessEpg();
	process->confdir = string(argv[1]) ; 
	
	for (n=2; n<argc; n++)
	{
		process->processFile(argv[n]);
	}
	delete process;
	return 0 ;
}
