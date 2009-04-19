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
	cProcessEpg::chanmap = new cChannelMap();
	cProcessEpg::datamap = new cDataMap();
}

cProcessEpg::~cProcessEpg()
{

}

void cProcessEpg::processNode(xmlTextReaderPtr reader, xmlTextWriterPtr writer, UserDataPtr &user_data) 
{
	UserDataPtr pud = user_data; 
	
	struct tm tm;
	xmlNodePtr node ;
	xmlChar *value;	
	int retval;
	
	int type = xmlTextReaderNodeType(reader) ;
	int depth = xmlTextReaderDepth(reader) ;
	const char *name = (char *)xmlTextReaderConstName(reader);

	if (type == XML_READER_TYPE_ELEMENT && depth == 2)
	{	
		node = xmlTextReaderExpand(reader);
		value = xmlXPathCastNodeToString(node) ; // get the content and ...
		node = NULL ; 
		retval = xmlTextReaderNext(reader); // move to closing tag on same level, we don't need to go deeper
		
		// decide where to put the value
		//     
		if (!strcmp(name,"d0")) pud->broadcast_id = xmlStrdup(value);
		else if (!strcmp(name,"d1")) pud->tvshow_id = xmlStrdup(value);
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
			if (atol((char *)value)) pud->primetime = xmlCharStrdup("|PrimeTime");
								else pud->primetime = xmlCharStrdup("");
		}
		else if (!strcmp(name,"d10")) {
			if (atol((char *)value)) pud->category = cProcessEpg::datamap->GetStr(atol((char *)value));
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
							"unknown live_id: %ld !\n", 
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
					fprintf(stderr, "unknown tipflag: %ld !\n", atol((char *)value));
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
			 if (atol((char *)value)) pud->genre = cProcessEpg::datamap->GetStr(atol((char *)value));
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
					fprintf(stderr, "unknown rating: %ld !\n", atol((char *)value));
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
		xmlFree(value); 
		value = NULL ; 
	}
	else if (type == XML_READER_TYPE_END_ELEMENT && depth == 1)
	{
		// One event finished (data end tag reached), lets print the event!
		//
		
		
		if (!pud->regional) {
			for (pud->chanindex = 0; pud->chanindex < cProcessEpg::chanmap->GetChanCnt(pud->tvchannel_id); pud->chanindex++)
			{
				// C: channelid channelname
				// S19.2E-1-1101-28106 Das Erste
				xmlTextWriterWriteFormatString(writer,"C %s\n", cProcessEpg::chanmap->GetChanStr(pud->tvchannel_id, pud->chanindex) );  
				
				// E: eventid starttime(unixdate) duration 0 0 
				// 37237569 1236067500 3000 0 0
				xmlTextWriterWriteFormatString(writer,"E %s %ld %d 50\n", pud->broadcast_id, pud->starttime, pud->tvshow_length);
				
				//T: title 
				xmlTextWriterWriteFormatString(writer,"T %s\n", pud->title);
				
				// subtitle(episodetitle or the like)
				xmlTextWriterWriteFormatString(writer,"S %s\n", pud->subtitle);
				
				//main text
				xmlTextWriterWriteFormatString(writer,"D ");
				xmlTextWriterWriteFormatString(writer,"%s",pud->stars);
				xmlTextWriterWriteFormatString(writer,"%s",pud->tip); 
				xmlTextWriterWriteFormatString(writer,"%s|", pud->comment_long);
				xmlTextWriterWriteFormatString(writer,"%s - %s",pud->category.c_str(), pud->genre.c_str());
				xmlTextWriterWriteFormatString(writer,"%s",pud->primetime);
				xmlTextWriterWriteFormatString(writer,"%s",pud->sequence);
				
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_bw);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_co_channel);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_vt150);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_coded);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_blind);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_stereo);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_dolby);
				xmlTextWriterWriteFormatString(writer,"%s",pud->technics_wide);
				
				xmlTextWriterWriteFormatString(writer,"%s",pud->age_marker);
				xmlTextWriterWriteFormatString(writer,"%s",pud->live);
				xmlTextWriterWriteFormatString(writer,"%s",pud->attribute);
				xmlTextWriterWriteFormatString(writer,"%s",pud->country);
				xmlTextWriterWriteFormatString(writer,"%s",pud->year);
				xmlTextWriterWriteFormatString(writer,"%s",pud->themes);
				xmlTextWriterWriteFormatString(writer,"%s",pud->moderator);
				xmlTextWriterWriteFormatString(writer,"%s",pud->studio_guest);
				xmlTextWriterWriteFormatString(writer,"%s",pud->regisseur);
				xmlTextWriterWriteFormatString(writer,"%s",pud->actor);
				xmlTextWriterWriteFormatString(writer,"%s",pud->tvshow_id);
				xmlTextWriterWriteFormatString(writer,"\n"); // end of D (main information section, line breaks are '|' (pipe) in here !
				if (pud->vps)
				{
					xmlTextWriterWriteFormatString(writer,"V %ld\n", pud->vps);
				}
				
				// end event and channel
				xmlTextWriterWriteFormatString(writer,"e\n");
				xmlTextWriterWriteFormatString(writer,"c\n");
			}
			
			// cleanup for next element
			xmlFree(pud->primetime);			pud->primetime = NULL ;
			xmlFree(pud->technics_bw);			pud->technics_bw = NULL;
			xmlFree(pud->technics_co_channel);	pud->technics_co_channel = NULL;
			xmlFree(pud->technics_vt150);		pud->technics_vt150 =NULL;
			xmlFree(pud->technics_coded);		pud->technics_coded = NULL;
			xmlFree(pud->technics_blind);		pud->technics_blind = NULL;
			xmlFree(pud->age_marker);			pud->age_marker = NULL;
			xmlFree(pud->live);					pud->live = NULL; 
			xmlFree(pud->tip);					pud->tip = NULL;
			xmlFree(pud->title);				pud->title = NULL;
			xmlFree(pud->subtitle);				pud->subtitle = NULL;
			xmlFree(pud->comment_long);			pud->comment_long = NULL;
			xmlFree(pud->comment_middle);		pud->comment_middle = NULL;
			xmlFree(pud->comment_short);		pud->comment_short = NULL;
			xmlFree(pud->themes);				pud->themes = NULL;
			xmlFree(pud->sequence);				pud->sequence = NULL;
			xmlFree(pud->stars);				pud->stars = NULL;
			xmlFree(pud->attribute);			pud->attribute = NULL;
			xmlFree(pud->technics_stereo);		pud->technics_stereo = NULL;
			xmlFree(pud->technics_dolby);		pud->technics_dolby = NULL;
			xmlFree(pud->technics_wide);		pud->technics_wide = NULL;
	 		xmlFree(pud->country);				pud->country = NULL;
			xmlFree(pud->year);					pud->year = NULL;
			xmlFree(pud->moderator);			pud->moderator = NULL;
			xmlFree(pud->studio_guest);			pud->studio_guest = NULL;
			xmlFree(pud->regisseur);			pud->regisseur = NULL;
			xmlFree(pud->actor);				pud->actor = NULL;
		} 
	}


}





int cProcessEpg::processFile(char *filename)
{
  struct zip *pzip;
  int num_files;
  int zipfilenum;
  struct zip_stat zstat;
  struct zip_file *zfile;
  int len;
  char *buffer;
  xmlTextReaderPtr reader;
  xmlTextWriterPtr writer;
  int parseretval;
  UserData ud;
  UserDataPtr user_data = &ud ;
  const char *fname;
  char *file = filename;
  string outfile = string(filename); 
	     outfile = outfile.substr(0,outfile.length() -4) + ".epg"; 
  
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
	  zip_fclose(zfile);
	  
		LIBXML_TEST_VERSION
		writer = xmlNewTextWriterFilename(outfile.c_str(), 0);
		if (writer == NULL) {
			printf("testXmlwriterFilename: Error creating the xml writer\n");
			return -27;
		}
		xmlTextWriterStartDocument(writer, NULL,"utf-8",NULL);
		

		reader = xmlReaderForMemory(buffer, zstat.size,"/root/test1/include/","iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
		if (reader != NULL)	{
	        parseretval = xmlTextReaderRead(reader);
	        while (parseretval == 1) {
	            processNode(reader, writer, user_data);
	            parseretval = xmlTextReaderRead(reader);
	        }
	        xmlFreeTextReader(reader);
			xmlFreeTextWriter(writer);
	        if (parseretval != 0) {
	            fprintf(stderr, "failed to parse %s,\n skipping rest of the file and cleanup\n",file);
					xmlFree(user_data->primetime);				user_data->primetime = NULL ;
					xmlFree(user_data->technics_bw);			user_data->technics_bw = NULL;
					xmlFree(user_data->technics_co_channel);	user_data->technics_co_channel = NULL;
					xmlFree(user_data->technics_vt150);			user_data->technics_vt150 =NULL;
					xmlFree(user_data->technics_coded);			user_data->technics_coded = NULL;
					xmlFree(user_data->technics_blind);			user_data->technics_blind = NULL;
					xmlFree(user_data->age_marker);				user_data->age_marker = NULL;
					xmlFree(user_data->live);					user_data->live = NULL; 
					xmlFree(user_data->tip);					user_data->tip = NULL;
					xmlFree(user_data->title);					user_data->title = NULL;
					xmlFree(user_data->subtitle);				user_data->subtitle = NULL;
					xmlFree(user_data->comment_long);			user_data->comment_long = NULL;
					xmlFree(user_data->comment_middle);			user_data->comment_middle = NULL;
					xmlFree(user_data->comment_short);			user_data->comment_short = NULL;
					xmlFree(user_data->themes);					user_data->themes = NULL;
					xmlFree(user_data->sequence);				user_data->sequence = NULL;
					xmlFree(user_data->stars);					user_data->stars = NULL;
					xmlFree(user_data->attribute);				user_data->attribute = NULL;
					xmlFree(user_data->technics_stereo);		user_data->technics_stereo = NULL;
					xmlFree(user_data->technics_dolby);			user_data->technics_dolby = NULL;
					xmlFree(user_data->technics_wide);			user_data->technics_wide = NULL;
			 		xmlFree(user_data->country);				user_data->country = NULL;
					xmlFree(user_data->year);					user_data->year = NULL;
					xmlFree(user_data->moderator);				user_data->moderator = NULL;
					xmlFree(user_data->studio_guest);			user_data->studio_guest = NULL;
					xmlFree(user_data->regisseur);				user_data->regisseur = NULL;
					xmlFree(user_data->actor);					user_data->actor = NULL;
	        }
		} 
		else fprintf(stderr, "Unable to get xml\n");
      xmlCleanupParser();
    }
}

  if (zip_close(pzip))
  {
    fprintf(stderr, "error: can't close zip file\n");
    return -9;
  }   
  free(buffer); buffer = NULL;
  return 0;
}
