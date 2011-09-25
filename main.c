/*
 * main.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */
#include <string>
#include <getopt.h>

#define PROCDIR "/var/cache/epgdata2vdr/"
#define CHANNELMAP "/etc/epgdata2vdr/channelmap.conf"
#define GENREMAP "/etc/epgdata2vdr/genremap.conf"
#define INCDIR "/var/cache/epgdata2vdr/include/"
#define IMAGESIZE 120

#include "update.h"
using namespace std;

void usage() {
    fprintf(stderr,"USAGE: epgdata2vdr [OPTIONS] filename [filename ... ] \n");
    fprintf(stderr,"\t-p\t--processing-directory <directory>\n\t\tfiles will be processed and kept for next run here.\n\t\t(Default: %s)\n\n",PROCDIR);
    fprintf(stderr,"\t-i\t--image-directory <directory>\n\t\timages will be linked with evend id, for use with vdr here.\n\n");
    fprintf(stderr,"\t-s\t--image-size int\n\t\timage size the pictures will be resized to\n\t\t(Default: %d)\n\n",IMAGESIZE );
    fprintf(stderr,"\t-I\t--include-directory <directory>\n\t\tpath to the directory containing the content from the include file of epgdata.com\n\t\t(Default:%s)\n\n",INCDIR);
    fprintf(stderr,"\t-c\t--channel-map <filename>\n\t\tfilename with path where the file with mapping between epgdata channel and vdr is located\n\t\t(Default:%s)\n\n",CHANNELMAP);
    fprintf(stderr,"\t-g\t--genre-map <filename>\n\t\tfilename with path where the file with mapping between epgdata genre and dvb-si genre for vdr is located\n\t\t(Default:%s)\n\n",GENREMAP);
    fprintf(stderr,"\t-f\t--image-format jpg|png\n\t\toutput format of the epgimages\n\n");
    fprintf(stderr,"\t-C\t--print-channels\n\t\tprint all mapped channels in noepgmenu format (channel id, space seperated)\n");
}

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
        { "print-channels", no_argument, NULL, 'C' },
        { "genre-map", required_argument, NULL, 'g' },
        { "help", no_argument, NULL, 'h' },
        {0, 0, 0, 0}
    };

bool printchan = false;

int main(int argc, char *argv[])
{
    int opt = 0;
    cProcessEpg *process ;

    process = new cProcessEpg();

    while(1)
    {
        int longIndex = 0;

        if ((opt = getopt_long( argc, argv, "i:p:s:o:I:c:f:g:hC", long_options, &longIndex )) == -1) {
            break;
        }

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

            case 'g':
                process->genremapfile = string(optarg);
                break;

            case 'C':
                printchan = true;
                break;

            case 'h':
            case '?':
                usage();
                return 1;
            default:
                break;
        }
    }
    // if not given set defaults for the required paths
    if ( process->incdir == "" )  process->incdir = INCDIR ;
    if ( process->procdir == "" ) process->procdir = PROCDIR ;
    if ( process->channelmapfile == "" ) process->channelmapfile = CHANNELMAP ;
    if ( process->genremapfile == "" ) process->genremapfile = GENREMAP ;
    if ( process->imageformat == "" ) {
        if ( process->imgsize == 0 )  process->imageformat = "jpg";
                      else process->imageformat = "png";
    } else {
        if ( process->imgsize == 0 ) process->imgsize = IMAGESIZE ;
    }

    // read genre and category from includedir and channelmap.
    process->chanmap = new cChannelMap(process->channelmapfile);
    process->datamap = new cDataMap(process->incdir);
    process->genremap = new cGenreMap(process->genremapfile);

    if (optind < argc) {

        // rest of the arguments are the files to parse
        while (optind < argc) {
            process->processFile(process->procdir, argv[optind++]);
        }
    } else { 
        if ( printchan ) {
            process->chanmap->GetAllChanStr();
        } else {
            fprintf(stderr,"ERROR: No file to be processed.\n");
            usage();
            return 1;
        }
    }

    // free the memory from processing data and exit
    delete process;
    return 0 ;
}
