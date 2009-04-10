#include <cstdlib>
#include <string>

#include "update.h" 
using namespace std;


int main(int argc, char *argv[])
{
	int n;
	cProcessEpg *process ; 
	process = new cProcessEpg();
	UserData user_data;  

	
	for (n=1; n<argc; n++)
	{
		int retval = process->processFile(&user_data , argv[n]);
	}
	return 0 ;
}
