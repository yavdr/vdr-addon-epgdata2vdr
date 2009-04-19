#include <cstdlib>
#include <string>

#include "update.h" 
using namespace std;


int main(int argc, char *argv[])
{
	int n;
	cProcessEpg *process ; 
	process = new cProcessEpg();
	
	for (n=1; n<argc; n++)
	{
		process->processFile(argv[n]);
	}
	return 0 ;
}
