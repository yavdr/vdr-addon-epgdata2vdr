/*
 * main.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */
#include <string>
#include <getopt.h>

#define PROCDIR "/var/cache/vdr/epgdata2vdr"
#define CHANNELMAP "/etc/vdr/channelmap_epgdata2vdr.conf"
#define INCDIR "/var/cache/vdr/epgdata2vdr/includes"

#include "update.h"
using namespace std;

void usage() {
    printf("usage: epgdata2vdr\n");
    printf("USAGE\n\tepgdata2vdr --processing-directory <directory> ... <filename> TODO \n");
    printf("\n\tepgdata2vdr [-h|--help]\n");
}

int main(int argc, char *argv[])
{
    int opt = 0;
	int longIndex = 0;
    cProcessEpg *process ;

	process = new cProcessEpg();

    static const char *optString = "ipsocIhf?";

    static struct option long_options[] =
    {
        { "image-directory", required_argument, NULL, 'i' },
        { "processing-directory", required_argument, NULL, 'p' },
#ifdef USE_IMAGEMAGICK
        { "image-size", required_argument, NULL, 's' },
#endif
        { "output-format", required_argument, NULL, 'o' },
        { "include-directory", required_argument, NULL, 'I' },
        { "channel-map", required_argument, NULL, 'c' },
        { "image-format", required_argument, NULL, 'f' },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 'h' }
    };

    while( (opt = getopt_long( argc, argv, optString, long_options, &longIndex )) != -1 )
    {
        switch( opt ) {
            case 'i':
                process->epgimagesdir = string(optarg);
                break;

            case 'p':
                process->procdir = string(optarg);
                break;
#ifdef USE_IMAGEMAGICK
            case 's':
                process->imgsize = atoi(optarg);
                break;
#endif
            case 'o':
                process->outfmt = string(optarg);
                break;

            case 'I':
                process->incdir = string(optarg);
                break;

            case 'c':
                process->channelmapfile = string(optarg);
                break;

            case 'f':
                process->imageformat = string(optarg);
                break;

            case 'h':   /* fall-through is intentional */
            case '?':
                usage();
                break;
            default:
                /* You won't actually get here. */
                break;
        }
    }

    if (longIndex == 0) {
        usage(); // at least something needs to be provided
        exit(1);
    }

    // if not given set defaults for the required paths
    if ( process->incdir == "" )  process->incdir = INCDIR ;
    if ( process->procdir == "" ) process->procdir = PROCDIR ;
    if ( process->channelmapfile == "" ) process->channelmapfile = CHANNELMAP ;
    if ( process->imageformat == "" ) {
        if ( process->imgsize == 0 )  process->imageformat = "jpg";
        else process->imageformat = "png";
    }

    // read genre and channelmap from includedir and channelmap.
	process->readMaps() ;

    // rest of the arguments are the files to parse
	for (optind = longIndex ; optind<argc; optind++)
	{
		process->processFile(process->procdir, argv[optind]);
	}

	// free the memory from processing data and exit
	delete process;
	return 0 ;
}
