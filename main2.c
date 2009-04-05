#include <cstdlib>
#include <string>

#include "update.h" 
using namespace std;


int main(int argc, char *argv[])
{
	cProcessEpg *process = new cProcessEpg;
	int retval = process->processFile(process->user_data , argv[1]);
	return retval;     
}
