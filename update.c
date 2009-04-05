/*
 * update.c: epgdata plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */
#include "update.h"

using namespace std;

cProcessEpg::cProcessEpg()
{
	UserDataPtr user_data;  
	user_data->chanmap = new cChannelMap;
	user_data->datamap = new cDataMap;
}

cProcessEpg::~cProcessEpg()
{

}

static void processNode(xmlTextReaderPtr reader, UserDataPtr &user_data) 
{
	UserDataPtr pud = user_data; 
	struct tm tm;
	const xmlChar *value;
	int retval;
	int type = xmlTextReaderNodeType(reader) ;
	int depth = xmlTextReaderDepth(reader) ;
	char *name = (char *)xmlTextReaderConstName(reader);


  if (type == XML_READER_TYPE_ELEMENT && depth == 2)
	{
		value = xmlXPathCastNodeToString(xmlTextReaderExpand(reader)); // get the content and ...
		retval = xmlTextReaderNext(reader); // move to closing tag on same level, we don't need to go deeper
		
		// decide where to put the value
		//     
		if (!strcmp(name,"d0")) pud->broadcast_id = atol((char *)value);
		else if (!strcmp(name,"d1")) pud->tvshow_id = atol((char *)value);
		else if (!strcmp(name,"d2")) pud->tvchannel_id = atol((char *)value); 
		else if (!strcmp(name,"d3")) pud->regional = atol((char *)value);
		else if (!strcmp(name,"d4")) {	// starttime -  next would be  d5,d6: endtime, broadcast_day not required
	    	sscanf((char *)value, "%04d-%02d-%02d %02d:%02d:%02d", &tm.tm_year, 
	     	&tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	    	tm.tm_year -= 1900;
	    	tm.tm_mon -= 1; 
			tm.tm_zone = strdup("CET"); // assume date in XML to be CET
	    	tm.tm_isdst = -1 ; // determine DST based on the time and locale 
	    	pud->starttime = mktime(&tm); //time_t as out of mktime is UTC on Linux
		}
		else if (!strcmp(name,"d7")) pud->tvshow_length = atol((char *)value) * 60;
		else if (!strcmp(name,"d8")) { // VPS
			if (strlen((char *)value) == 5) {
			  gmtime_r(&pud->starttime, &tm);
			  sscanf((char *)value, "%02d:%02d", &tm.tm_hour, &tm.tm_min);
			  tm.tm_sec = 0;
			  pud->vps = mktime(&tm);
			}
			else pud->vps = 0;
		}
		else if (!strcmp(name,"d9")) {
			if (atol((char *)value)) pud->primetime = string("|PrimeTime");
								else pud->primetime = string("");
		}
		else if (!strcmp(name,"d10")) {
			if (atol((char *)value)) pud->category = pud->datamap->GetStr(atol((char *)value));
								else pud->category = string("");
		}
		else if (!strcmp(name,"d11")) {
			if (atol((char *)value)) pud->technics_bw = xmlCharStrdup("Schwarz Weiß,");
								else pud->technics_bw = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d12")) {
			if (atol((char *)value)) pud->technics_co_channel = xmlCharStrdup("Zweikanalton,");
								else pud->technics_co_channel = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d13")) {
			if (atol((char *)value)) pud->technics_vt150 = xmlCharStrdup("Untertitel,");
								else pud->technics_vt150 = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d14")) {
			if (atol((char *)value)) pud->technics_coded = xmlCharStrdup("PayTV,");
								else pud->technics_coded = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d15")) {
			if (atol((char *)value)) pud->technics_blind = xmlCharStrdup("Hörfilm,");
								else pud->technics_blind = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d16")) {
			if (strlen((char *)value)) {
				pud->age_marker = xmlCharStrdup("|FSK: ");
				xmlStrcat(pud->age_marker, value); 
			}
			else pud->age_marker = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d17")) {
			switch(atol((char *)value)) {
				case 0: pud->live = xmlCharStrdup("");				break;
				case 1: pud->live = xmlCharStrdup("|Live");			break;
				case 2:	pud->live = xmlCharStrdup("|Wiederholung");	break;
				case 3: pud->live = xmlCharStrdup("|Zeitversetzte Übertragung");	break;
				default:
					pud->live = xmlCharStrdup("") ;
					fprintf(stderr, 
							"unknown live_id: %d !\n", 
							atol((char *)value));
			}
		}
		else if (!strcmp(name,"d18")) {
			switch(atol((char *)value)) {
				case 0: pud->tip = xmlCharStrdup(""); 				break;
				case 1: pud->tip = xmlCharStrdup("[Spartentipp]");	break;
				case 2:	pud->tip = xmlCharStrdup("[Genretipp]");	break;
				case 3:	pud->tip = xmlCharStrdup("[Tagestipp]");	break;
				default: 
					pud->tip = xmlCharStrdup("");
					fprintf(stderr, "unknown tipflag: %d !\n", atol((char *)value));
			}
		}
		else if (!strcmp(name,"d19")) pud->title = xmlStrdup(value);
		else if (!strcmp(name,"d20")) pud->subtitle = xmlStrdup(value);
		else if (!strcmp(name,"d21")) pud->comment_long = xmlStrdup(value);
		else if (!strcmp(name,"d22")) pud->comment_middle = xmlStrdup(value);
		else if (!strcmp(name,"d23")) pud->comment_short = xmlStrdup(value);
		else if (!strcmp(name,"d24")) {
			if (xmlStrlen(value)) {
				pud->themes = xmlCharStrdup("|");
				pud->themes = xmlStrcat(pud->themes,value);
			}
			else pud->themes = xmlCharStrdup("");
		}		
		else if (!strcmp(name,"d25")) {
			 if (atol((char *)value)) pud->genre = pud->datamap->GetStr(atol((char *)value));
								 else pud->genre = string("");
		}
		else if (!strcmp(name,"d26")) {
			if (xmlStrlen(value)) {
				pud->sequence = xmlCharStrdup("|Folge: ");
				pud->sequence = xmlStrcat(pud->sequence,value);
			}
			else pud->sequence = xmlCharStrdup("");
		}		
		else if (!strcmp(name,"d27")) {
			if (atol((char *)value)) pud->technics_stereo = xmlCharStrdup("Stereo,");
								else pud->technics_stereo = xmlCharStrdup("");
		}	
		else if (!strcmp(name,"d28")) {
			if (atol((char *)value)) pud->technics_dolby =  xmlCharStrdup("Dolby Digital,");
								else pud->technics_dolby =  xmlCharStrdup("");
		}
		else if (!strcmp(name,"d29")) {
			if (atol((char *)value)) pud->technics_wide =  xmlCharStrdup("16:9,");
								else pud->technics_wide =  xmlCharStrdup("");
		}		
		else if (!strcmp(name,"d30")) {
			switch(atol((char *)value))	{
				case 0: pud->stars =  xmlCharStrdup("");			break;
				case 1: pud->stars =  xmlCharStrdup("[*----] ");	break;
				case 2: pud->stars =  xmlCharStrdup("[**---] ");	break;
				case 3: pud->stars =  xmlCharStrdup("[***--] ");	break;
				case 4:	pud->stars =  xmlCharStrdup("[****-] "); 	break;
				case 5:	pud->stars =  xmlCharStrdup("[*****] ");	break;
				default: 
					pud->stars =  	  xmlCharStrdup("");
					fprintf(stderr, "unknown rating: %d !\n", atol((char *)value));
			}
		} 
		else if (!strcmp(name,"d31")) {
			if (xmlStrlen(value)) {
				pud->attribute = xmlCharStrdup("|Prädikat: ");
				pud->attribute = xmlStrcat(pud->attribute,value);
			}
			else pud->attribute = xmlCharStrdup("");
		}		
		else if (!strcmp(name,"d32")) {
			if (xmlStrlen(value)) {
				pud->country = xmlCharStrdup("|Land: ");
				pud->country = xmlStrcat(pud->country,value);
			}
			else pud->country = xmlCharStrdup("");
		}	
		else if (!strcmp(name,"d33")) {
			if (xmlStrlen(value)) {
				pud->year = xmlCharStrdup("|Jahr: ");
				pud->year = xmlStrcat(pud->year,value);
			}
			else pud->year = xmlCharStrdup("");
		}		
		else if (!strcmp(name,"d34")) {
			if (xmlStrlen(value)) {
				pud->moderator = xmlCharStrdup("|Moderator: ");
				pud->moderator = xmlStrcat(pud->moderator,value);
			}
			else pud->moderator = xmlCharStrdup("");
		}	
		else if (!strcmp(name,"d35")) {
			if (xmlStrlen(value)) {
				pud->studio_guest = xmlCharStrdup("|Gast: ");
				pud->studio_guest = xmlStrcat(pud->studio_guest,value);
			}
			else pud->studio_guest = xmlCharStrdup("");
		}	
		else if (!strcmp(name,"d36")) {
			if (xmlStrlen(value)) {
				pud->regisseur = xmlCharStrdup("|Regie: ");
				pud->regisseur = xmlStrcat(pud->regisseur,value);
			}
			else pud->regisseur = xmlCharStrdup("");
		}	
		else if (!strcmp(name,"d37")) {
			if (xmlStrlen(value)) {
				pud->actor = xmlCharStrdup("|Schauspieler: ");
				pud->actor = xmlStrcat(pud->actor,value);
			}
			else pud->actor = xmlCharStrdup("");
		}	
		
			// d38, d39 	-	Bilder existieren nicht (image_small, image_medium)
			// image_big		d40		Bildverarbeitung !
			
	}
	else if (type == 15 && depth == 1)
	{
		// One event finished (data end tag reached), lets print the event!
		//
		if (!pud->regional) {
			for (pud->chanindex = 0; pud->chanindex < pud->chanmap->GetChanCnt(pud->tvchannel_id); pud->chanindex++)
			{
				// C: channelid channelname
				// S19.2E-1-1101-28106 Das Erste
				printf("C %s\n", pud->chanmap->GetChanStr(pud->tvchannel_id, pud->chanindex) );  
				
				// E: eventid starttime(unixdate) duration 0 0 
				// 37237569 1236067500 3000 0 0
				printf("E %d %d %d 50\n", pud->broadcast_id, pud->starttime, pud->tvshow_length);
				
				//T: title 
				printf("T %s\n", pud->title);
				
				// subtitle(episodetitle or the like)
				printf("S %s\n", pud->subtitle);
				
				//main text
				printf("D ");
				printf("%s",pud->stars);
				printf("%s",pud->tip); 
				printf("%s|", pud->comment_long);
				printf("%s - %s",pud->category.c_str(), pud->genre.c_str());
				printf("%s",pud->primetime.c_str());
				printf("%s",pud->sequence);
				
				printf("%s",pud->technics_bw);
				printf("%s",pud->technics_co_channel);
				printf("%s",pud->technics_vt150);
				printf("%s",pud->technics_coded);
				printf("%s",pud->technics_blind);
				printf("%s",pud->technics_stereo);
				printf("%s",pud->technics_dolby);
				printf("%s",pud->technics_wide);
				
				printf("%s",pud->age_marker);
				printf("%s",pud->live);
				printf("%s",pud->attribute);
				printf("%s",pud->country);
				printf("%s",pud->year);
				printf("%s",pud->themes);
				printf("%s",pud->moderator);
				printf("%s",pud->studio_guest);
				printf("%s",pud->regisseur);
				printf("%s",pud->actor);
				printf("\n"); // end of D (main information section, line breaks are '|' (pipe) in here !
				if (pud->vps)
				{
					printf("V %d\n", pud->vps);
				}
				
				// end event and channel
				printf("e\n");
				printf("c\n");
			}
			
			// cleanup for next element
			free(pud->technics_bw);			pud->technics_bw = NULL;
			free(pud->technics_co_channel);	pud->technics_co_channel = NULL;
			free(pud->technics_vt150);		pud->technics_vt150 =NULL;
			free(pud->technics_coded);		pud->technics_coded = NULL;
			free(pud->technics_blind);		pud->technics_blind = NULL;
			free(pud->age_marker);			pud->age_marker = NULL;
			free(pud->live);				pud->live = NULL; 
			free(pud->tip);					pud->tip = NULL;
			free(pud->title);				pud->title = NULL;
			free(pud->subtitle);			pud->subtitle = NULL;
			free(pud->comment_long);		pud->comment_long = NULL;
			free(pud->comment_middle);		pud->comment_middle = NULL;
			free(pud->comment_short);		pud->comment_short = NULL;
			free(pud->themes);				pud->themes = NULL;
			free(pud->sequence);			pud->sequence = NULL;
			free(pud->stars);				pud->stars = NULL;
			free(pud->attribute);			pud->attribute = NULL;
			free(pud->technics_stereo);		pud->technics_stereo = NULL;
			free(pud->technics_dolby);		pud->technics_dolby = NULL;
			free(pud->technics_wide);		pud->technics_wide = NULL;
	 		free(pud->country);				pud->country = NULL;
			free(pud->year);				pud->year = NULL;
			free(pud->moderator);			pud->moderator = NULL;
			free(pud->studio_guest);		pud->studio_guest = NULL;
			free(pud->regisseur);			pud->regisseur = NULL;
			free(pud->actor);				pud->actor = NULL;
		} 
	}
}





int processFile(UserDataPtr userdata, char *filename)
{
  struct zip *pzip;
  int num_files;
  int zipfilenum;
  const char *fname;
  struct zip_stat zstat;
  struct zip_file *zfile;
  int len;
  char *buffer;
  xmlTextReaderPtr reader;
  int parseretval;
  UserDataPtr user_data = userdata ;
  char *file = filename;

  
  if ((pzip = zip_open(file, 0, NULL)) == NULL)
  {
    fprintf(stderr, "error: can't open %s\n", file);
    return -2;
  }
  
  num_files = zip_get_num_files(pzip);
  for (zipfilenum = 0; zipfilenum < num_files; zipfilenum++)
  {
    if ((fname = zip_get_name(pzip, zipfilenum, 0)) == NULL) {
      fprintf(stderr, "error: can't get filename for index %d\n", zipfilenum);
      return -3;
    } 
    if (!strcmp(fname + strlen(fname) - 4, ".xml")) {
      if (zip_stat_index(pzip, zipfilenum, 0, &zstat)) {
        fprintf(stderr, "error: can't get stat for %s\n", fname);
        return -4;
      }
      if ((buffer = (char *)malloc(zstat.size)) == NULL) {
        fprintf(stderr, "error: can't get enough memory\n");
        return -5;
      }
      if ((zfile = zip_fopen_index(pzip, zipfilenum, 0)) == NULL) {
        fprintf(stderr, "error: can't can't open zip file %s\n", fname);
        return -7;
      }
	  
	  // fill buffer from xml
	  len = zip_fread(zfile, buffer, zstat.size);
	  LIBXML_TEST_VERSION
	  reader = xmlReaderForMemory(buffer, zstat.size,"/root/test1/include/","iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
		if (reader != NULL)	{
	        parseretval = xmlTextReaderRead(reader);
	        while (parseretval == 1) {
	            processNode(reader, user_data);
	            parseretval = xmlTextReaderRead(reader);
	        }
	        xmlFreeTextReader(reader);
	        if (parseretval != 0) {
	            fprintf(stderr, "failed to parse\n");
	        }
		} 
		else fprintf(stderr, "Unable to get xml\n");
      xmlCleanupParser();
      zip_fclose(zfile);
    }
}

  if (zip_close(pzip))
  {
    fprintf(stderr, "error: can't close zip file\n");
    return -9;
  }   

  return 0;
}
