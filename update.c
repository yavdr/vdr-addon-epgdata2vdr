/*
 * update.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */
#include "update.h"

using namespace std;

cProcessEpg::cProcessEpg()
{
    LIBXML_TEST_VERSION
#ifdef USE_IMAGEMAGICK
    MagickCoreGenesis("epgdata2vdr-im",MagickFalse); // new image magick initialize
#endif
    imageCount = 0;
}

cProcessEpg::~cProcessEpg()
{
    xmlCleanupParser();
#ifdef USE_IMAGEMAGICK
    MagickCoreTerminus();
#endif
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
    if (!strcmp(name,"d0")) {
       pud->broadcast_id = xmlStrdup(value);
       pud->item = atol((char *)value);
    }
    else if (!strcmp(name,"d1")) pud->tvshow_id = xmlStrdup(value);
    else if (!strcmp(name,"d2")) pud->tvchannel_id = atol((char *)value);
    else if (!strcmp(name,"d3")) pud->regional = atol((char *)value);
    else if (!strcmp(name,"d4")) {  // starttime -  next would be  d5,d6: endtime, broadcast_day not required
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
      if (atol((char *)value)) pud->primetime = xmlCharStrdup("PrimeTime");
                else pud->primetime = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d10")) {
      if (atol((char *)value)) pud->category = cProcessEpg::datamap->GetStr(atoi((char *)value));
                else pud->category = string("");
    }
    else if (!strcmp(name,"d11")) {
      if (atol((char *)value)) pud->technics_bw = xmlCharStrdup("Schwarz Weiß");
                else pud->technics_bw = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d12")) {
      if (atol((char *)value)) pud->technics_co_channel = xmlCharStrdup("Zweikanalton");
                else pud->technics_co_channel = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d13")) {
      if (atol((char *)value)) pud->technics_vt150 = xmlCharStrdup("Untertitel");
                else pud->technics_vt150 = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d14")) {
      if (atol((char *)value)) pud->technics_coded = xmlCharStrdup("PayTV");
                else pud->technics_coded = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d15")) {
      if (atol((char *)value)) pud->technics_blind = xmlCharStrdup("Hörfilm");
                else pud->technics_blind = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d16")) {
      if (xmlStrlen(value)) {
        pud->age_marker = xmlStrdup(value);
      }
      else {
        pud->age_marker = xmlCharStrdup("");
      }
    }
    else if (!strcmp(name,"d17")) {
      switch(atol((char *)value)) {
        case 0: pud->live = xmlCharStrdup("");        break;
        case 1: pud->live = xmlCharStrdup("Live");      break;
        case 2: pud->live = xmlCharStrdup("Wiederholung");  break;
        case 3: pud->live = xmlCharStrdup("Zeitversetzte Übertragung"); break;
        default:
          pud->live = xmlCharStrdup("") ;
          fprintf(stderr,
              "unknown live_id: %ld !\n",
              atol((char *)value));
      }
    }
    else if (!strcmp(name,"d18")) {
      if (xmlStrlen(value)) {
        pud->tip = atoi((char *)value) ;
      }
      else {
        pud->tip = 0 ;
      }
    }
    else if (!strcmp(name,"d19")) pud->title = xmlStrdup(value);
    else if (!strcmp(name,"d20")) pud->subtitle = xmlStrdup(value);
    else if (!strcmp(name,"d21")) { // comment_long
      if (xmlStrlen(value)) {
        size_t size = xmlStrlen(value);
        for (size_t i = 0; i < size; ++i) { //Replacing the \n and \r with |
            if ((value[i] == '\n') || (value[i] == '\r')) value[i] = '|';
				}
			}
		pud->comment_long = xmlStrdup(value);
		}

    else if (!strcmp(name,"d22")) { // comment_middle
      if (xmlStrlen(value)) {
				size_t size = xmlStrlen(value);
        for (size_t i = 0; i < size; ++i) { //Replacing the \n and \r with |
            if ((value[i] == '\n') || (value[i] == '\r')) value[i] = '|';
				}
			}
		pud->comment_middle = xmlStrdup(value);
		}

		else if (!strcmp(name,"d23")) { // comment_short
      if (xmlStrlen(value)) {
				size_t size = xmlStrlen(value);
        for (size_t i = 0; i < size; ++i) { //Replacing the \n and \r with |
            if ((value[i] == '\n') || (value[i] == '\r')) value[i] = '|';
				}
			}
		pud->comment_short = xmlStrdup(value);
		}

    else if (!strcmp(name,"d24")) {
      if (xmlStrlen(value)) {
        pud->themes = xmlCharStrdup("|");
        pud->themes = xmlStrcat(pud->themes,value);
      }
      else pud->themes = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d25")) {
        if (atol((char *)value)) pud->genre_id = atol((char *)value);
    }
    else if (!strcmp(name,"d26")) {
      if ((xmlStrlen(value) != 0) && (atol((char *)value) != 0)) {
        pud->sequence = xmlStrdup(value);
      }
      else pud->sequence = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d27")) {
      if (atol((char *)value)) pud->technics_stereo = xmlCharStrdup("Stereo");
                else pud->technics_stereo = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d28")) {
      if (atol((char *)value)) pud->technics_dolby =  xmlCharStrdup("DolbyDigital");
                else pud->technics_dolby =  xmlCharStrdup("");
    }
    else if (!strcmp(name,"d29")) {
      if (atol((char *)value)) pud->technics_wide =  xmlCharStrdup("16:9");
                else pud->technics_wide =  xmlCharStrdup("");
    }
    else if (!strcmp(name,"d30")) {
      switch(atol((char *)value)) {
        case 0: pud->stars =  xmlCharStrdup("");      break;
        case 1: pud->stars =  xmlCharStrdup("[*] ");  break;
        case 2: pud->stars =  xmlCharStrdup("[**] ");  break;
        case 3: pud->stars =  xmlCharStrdup("[***] ");  break;
        case 4: pud->stars =  xmlCharStrdup("[****] ");  break;
        case 5: pud->stars =  xmlCharStrdup("[*****] ");  break;
        default:
          pud->stars =      xmlCharStrdup("");
          fprintf(stderr, "unknown rating: %ld !\n", atol((char *)value));
      }
    }
    else if (!strcmp(name,"d31")) {
      if (xmlStrlen(value)) {
        pud->attribute = xmlCharStrdup("Prädikat: ");
        pud->attribute = xmlStrcat(pud->attribute,value);
      }
      else pud->attribute = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d32")) {
      if (xmlStrlen(value)) {
        size_t size = xmlStrlen(value); // Replace "|" with "/"
        for (size_t i = 0; i < size; ++i) {
            if (value[i] == '|') value[i] = '/'; // Result = "USA/GB/D"
        }
        pud->country = xmlStrdup(value);
      }
      else pud->country = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d33")) {
      if (xmlStrlen(value)) {
        pud->year = xmlStrdup(value);
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
        pud->studio_guest = xmlStrdup(value);
      }
      else pud->studio_guest = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d36")) {
      if (xmlStrlen(value)) {
        pud->regisseur = xmlStrdup(value);
      }
      else pud->regisseur = xmlCharStrdup("");
    }
    else if (!strcmp(name,"d37")) {
      if (xmlStrlen(value)) {
        pud->actor = xmlStrdup(value);
      }
      else pud->actor = xmlCharStrdup("");
    }
    // d38, d39   - pictures don't exist ! (image_small, image_medium)
    else if (!strcmp(name,"d40")) {
      if (xmlStrlen(value)) {
        string pic = procdir + "/images/" + string((char *)value);
        pud->sourcepic = pic.substr(0,pic.length() - 4) + "." + imageformat ;
      }
    }
    xmlFree(value);
    value = NULL ;
  }
  else if (type == XML_READER_TYPE_END_ELEMENT && depth == 1)
  {
    // One event finished (data end tag reached), lets print the event!
    // Don't print regional and duplicate events
    //

    if (!pud->regional && (importedItems.find(pud->item) == importedItems.end()) ) {
      // we didn't had this event yet - remember we have worked on it 
      importedItems.insert(pud->item); 

      for (pud->chanindex = 0; pud->chanindex < cProcessEpg::chanmap->GetChanCnt(pud->tvchannel_id); pud->chanindex++)
      {
        // C: channelid channelname
        // S19.2E-1-1101-28106 Das Erste
        xmlTextWriterWriteFormatString(writer,"C %s\n", cProcessEpg::chanmap->GetChanStr(pud->tvchannel_id, pud->chanindex) );

        // E: eventid starttime(unixdate) duration 0 0
        // 37237569 1236067500 3000 0 0
        xmlTextWriterWriteFormatString(writer,"E %s %ld %d 0\n", pud->broadcast_id, pud->starttime, pud->tvshow_length);

        //T: title
        xmlTextWriterWriteFormatString(writer,"T %s\n", pud->title);

        // subtitle(episodetitle or the like)
        xmlTextWriterWriteFormatString(writer,"S %s\n", pud->subtitle);

        //main text
        xmlTextWriterWriteFormatString(writer,"D ");
        xmlTextWriterWriteFormatString(writer,"%s",pud->stars);
        switch(pud->tip) {
            case 0: break;
            case 1: xmlTextWriterWriteFormatString(writer,"[Spartentipp %s] ",pud->category.c_str());
              break;
            case 2: xmlTextWriterWriteFormatString(writer,"[Genretipp %s] ",cProcessEpg::datamap->GetStr(pud->genre_id).c_str());
              break;
            case 3: xmlTextWriterWriteFormatString(writer,"[Tagestipp] ");
              break;
            default:
              fprintf(stderr, "unknown tipflag: %d !\n", pud->tip);
          }
        if (xmlStrlen(pud->themes) > 0) xmlTextWriterWriteFormatString(writer,"Thema: %s|",pud->themes);
        if (xmlStrlen(pud->studio_guest) > 0)  xmlTextWriterWriteFormatString(writer,"Gäste: %s|",pud->studio_guest);
        if (xmlStrlen(pud->comment_short) != xmlStrlen(pud->comment_long) && xmlStrlen(pud->comment_short) > 0 ){
          xmlTextWriterWriteFormatString(writer,"Zusammenfassung: %s||", pud->comment_short);
        }
        if (xmlStrlen(pud->comment_long) > 0) xmlTextWriterWriteFormatString(writer,"%s", pud->comment_long);

        if (pud->category.size() > 0) {
          xmlTextWriterWriteFormatString(writer,"|Kategorie: %s",pud->category.c_str());
        } 

        if (cProcessEpg::datamap->GetStr(pud->genre_id).size() > 0) {
          xmlTextWriterWriteFormatString(writer,"|Genre: %s",cProcessEpg::datamap->GetStr(pud->genre_id).c_str());
        }

        if (xmlStrlen(pud->sequence) > 0) xmlTextWriterWriteFormatString(writer,"|Folge: %s",pud->sequence);

        if (xmlStrlen(pud->primetime) > 0) xmlTextWriterWriteFormatString(writer,"|%s",pud->primetime);

        if (xmlStrlen(pud->year) > 0 && xmlStrlen(pud->country) > 0) {
          xmlTextWriterWriteFormatString(writer,"|%s %s.",pud->country, pud->year);  // D 2005. 45 Min.
        } else if (xmlStrlen(pud->country) > 0) {
          xmlTextWriterWriteFormatString(writer,"|%s.",pud->country); // D. 45 Min.
        } else if (xmlStrlen(pud->year) > 0 ) {
          xmlTextWriterWriteFormatString(writer,"|%s.",pud->year); // 2005. 45 Min.
        }
        if (xmlStrlen(pud->country) > 0 || xmlStrlen(pud->year) > 0) xmlTextWriterWriteFormatString(writer," %d Min.",(pud->tvshow_length/60));
        else xmlTextWriterWriteFormatString(writer,"|%d Min.",(pud->tvshow_length/60));

        if ((xmlStrlen(pud->technics_bw) > 0) ||
          (xmlStrlen(pud->technics_co_channel) > 0) ||
          (xmlStrlen(pud->technics_coded) > 0)  ||
          (xmlStrlen(pud->technics_blind) > 0)  ||
          (xmlStrlen(pud->technics_stereo) > 0) ||
          (xmlStrlen(pud->technics_dolby) > 0)  ||
          (xmlStrlen(pud->technics_wide) > 0) ) {
          xmlTextWriterWriteFormatString(writer,"|Technische Details: ");
            if (xmlStrlen(pud->technics_bw) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_bw);
            if (xmlStrlen(pud->technics_co_channel) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_co_channel);
            if (xmlStrlen(pud->technics_vt150) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_vt150);
            if (xmlStrlen(pud->technics_coded) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_coded);
            if (xmlStrlen(pud->technics_blind) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_blind);
            if (xmlStrlen(pud->technics_stereo) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_stereo);
            if (xmlStrlen(pud->technics_dolby) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_dolby);
            if (xmlStrlen(pud->technics_wide) > 0) xmlTextWriterWriteFormatString(writer,"%s ",pud->technics_wide);
        }
        if (xmlStrlen(pud->age_marker) > 0) xmlTextWriterWriteFormatString(writer,"|FSK: %s",pud->age_marker);
        if (xmlStrlen(pud->live) > 0) xmlTextWriterWriteFormatString(writer,"|%s",pud->live);
        if (xmlStrlen(pud->attribute) > 0) xmlTextWriterWriteFormatString(writer,"|%s",pud->attribute);
        xmlTextWriterWriteFormatString(writer,"%s",pud->moderator);
        xmlTextWriterWriteFormatString(writer,"%s",pud->regisseur);
        xmlTextWriterWriteFormatString(writer,"%s",pud->actor);
        xmlTextWriterWriteFormatString(writer,"|Show-Id: %s",pud->tvshow_id);
        xmlTextWriterWriteFormatString(writer,"\n"); // end of D (main information section, line breaks are '|' (pipe) in here !

        // additional tags 
        if (pud->vps)
        {
          xmlTextWriterWriteFormatString(writer,"V %ld\n", pud->vps);
        }
        // parental rating - age
        if (xmlStrlen(pud->age_marker) > 0) xmlTextWriterWriteFormatString(writer,"R %s\n",pud->age_marker);
        
        // dvb-si genre
        if (cProcessEpg::genremap->GetGenreCount(pud->genre_id) > 0) { 
            xmlTextWriterWriteFormatString(writer,"G ");
            for (pud->genreindex = 0; pud->genreindex < cProcessEpg::genremap->GetGenreCount(pud->genre_id); pud->genreindex++)
            {
                xmlTextWriterWriteFormatString(writer,"%s", cProcessEpg::genremap->GetGenreString(pud->genre_id, pud->genreindex) );
                if ((pud->genreindex + 1) != cProcessEpg::genremap->GetGenreCount(pud->genre_id))
                    xmlTextWriterWriteFormatString(writer," ");
                else 
                    xmlTextWriterWriteFormatString(writer,"\n");
            }
        }

        // stream information (X values)
        //

        // structured additional information
        if (xmlStrlen(pud->regisseur) > 0) xmlTextWriterWriteFormatString(writer,"K REGISSEUR %s\n",pud->regisseur);
        if (xmlStrlen(pud->actor) > 0) xmlTextWriterWriteFormatString(writer,"K ACTORS %s\n",pud->actor);
        if (xmlStrlen(pud->year) > 0) xmlTextWriterWriteFormatString(writer,"K YEAR %s\n",pud->year);
        if (xmlStrlen(pud->country) > 0) xmlTextWriterWriteFormatString(writer,"K COUNTRY %s\n",pud->country);
        if (xmlStrlen(pud->sequence) > 0) xmlTextWriterWriteFormatString(writer,"K SEQUENCE %s\n",pud->sequence);
        

        // end event and channel
        xmlTextWriterWriteFormatString(writer,"e\n");
        xmlTextWriterWriteFormatString(writer,"c\n");
        if ((pud->sourcepic.length() > 0) && (epgimagesdir != "")) {
            string destpic = epgimagesdir + "/" + string((char *)pud->broadcast_id) + "." + imageformat;
            symlink(pud->sourcepic.c_str(),destpic.c_str());
        }
      }

      // cleanup for next element
      xmlFree(pud->primetime);      pud->primetime = NULL ;
      xmlFree(pud->technics_bw);      pud->technics_bw = NULL;
      xmlFree(pud->technics_co_channel);  pud->technics_co_channel = NULL;
      xmlFree(pud->technics_vt150);   pud->technics_vt150 =NULL;
      xmlFree(pud->technics_coded);   pud->technics_coded = NULL;
      xmlFree(pud->technics_blind);   pud->technics_blind = NULL;
      xmlFree(pud->age_marker);     pud->age_marker = NULL;
      xmlFree(pud->live);         pud->live = NULL;
      pud->tip = 0;
      xmlFree(pud->title);        pud->title = NULL;
      xmlFree(pud->subtitle);       pud->subtitle = NULL;
      xmlFree(pud->comment_long);     pud->comment_long = NULL;
      xmlFree(pud->comment_middle);   pud->comment_middle = NULL;
      xmlFree(pud->comment_short);    pud->comment_short = NULL;
      xmlFree(pud->themes);       pud->themes = NULL;
      xmlFree(pud->sequence);       pud->sequence = NULL;
      xmlFree(pud->stars);        pud->stars = NULL;
      xmlFree(pud->attribute);      pud->attribute = NULL;
      xmlFree(pud->technics_stereo);    pud->technics_stereo = NULL;
      xmlFree(pud->technics_dolby);   pud->technics_dolby = NULL;
      xmlFree(pud->technics_wide);    pud->technics_wide = NULL;
      xmlFree(pud->country);        pud->country = NULL;
      xmlFree(pud->year);         pud->year = NULL;
      xmlFree(pud->moderator);      pud->moderator = NULL;
      xmlFree(pud->studio_guest);     pud->studio_guest = NULL;
      xmlFree(pud->regisseur);      pud->regisseur = NULL;
      xmlFree(pud->actor);        pud->actor = NULL;
      pud->sourcepic = "" ;
    }
  }


}





int cProcessEpg::processFile(string confdir , char *filename)
{
  // unzip
  int num_files;
  int zipfilenum;
  struct zip *pzip;
  struct zip_stat zstat;
  struct zip_file *zfile;
  char *buffer;
  const char *fname;

  // parse
  xmlCharEncodingHandlerPtr encoder;
  xmlOutputBufferPtr outdocbuffer ;
  xmlTextReaderPtr reader;
  xmlTextWriterPtr writer;
  int parseretval;

  // temp store
  UserData ud;
  UserDataPtr user_data = &ud ;

  // in/output -  filename e.g.: 20091014_20091009_de_qy.zip
  string file = string(basename(filename));
  string outfile = procdir + file.substr(0,file.length() -4) + ".epg";

  // TODO: make it more error tolerant
  struct stat ds;
  if (!stat((procdir + "/images/").c_str(), &ds) == 0) {
   if (mkdir((procdir + "/images/").c_str(), ACCESSPERMS) == -1) {
    fprintf(stderr, "Unable to create picture directory %s.\n", (procdir + "/images/").c_str());
    return -2;
  }
  }

  if ((pzip = zip_open(filename, 0, NULL)) == NULL)
  {
    fprintf(stderr, "error: can't open zip file: %s\n", file.c_str());
    return -2;
  }

  num_files = zip_get_num_files(pzip); // get number of files in zip to iterate

  // first get dtd, then pictures then xml

  // extract the dtd
  for (zipfilenum = 0; zipfilenum < num_files; zipfilenum++) {
     if ((fname = zip_get_name(pzip, zipfilenum, 0)) == NULL) { // get the filename
         fprintf(stderr, "error: can't get filename for index %d\n", zipfilenum);
         return -3;
     }

     if (!strcmp(fname + strlen(fname) - 4, ".dtd")) {
        string dtdname = procdir + "/" + string(fname);
        if (zip_stat_index(pzip, zipfilenum, 0, &zstat)) {
           fprintf(stderr, "error: can't get stat for %s\n", fname);
           return -4;
        }
        if ((buffer = (char *)malloc(zstat.size)) == NULL) {
           fprintf(stderr, "error: can't get enough memory\n");
           return -5;
        }
        if ((zfile = zip_fopen_index(pzip, zipfilenum, 0)) == NULL) {
           fprintf(stderr, "error: can't open zip file %s\n", fname);
           return -7;
        }

        // fill buffer from dtd
        if (zip_fread(zfile, buffer, zstat.size) == -1 ) {
           fprintf(stderr, "could not extract dtd file from %s.\n", dtdname.c_str());
        };
        zip_fclose(zfile);

        FILE *fh1 = NULL;
        if ((fh1 = fopen(dtdname.c_str(), "w"))) {
           fwrite(buffer, 1, zstat.size, fh1);
           fclose(fh1);
        } else {
           fprintf(stderr, "could not write dtd file to confdir.\n");
        }
	free(buffer); buffer = NULL;
     } // if dtd
  } // loop through zip file for dtd

  // extract the pictures
  for (zipfilenum = 0; zipfilenum < num_files; zipfilenum++) {
     if ((fname = zip_get_name(pzip, zipfilenum, 0)) == NULL) { // get the filename
        fprintf(stderr, "error: can't get filename for index %d\n", zipfilenum);
        return -3;
     }

     if (!strcmp(fname + strlen(fname) - 4, ".jpg")) { // if we have a picture
         imageCount++ ;
         string outpic = procdir + "images/" + string(fname).substr(0,string(fname).length() -4) + "." + imageformat;
         struct stat pic;
         if ( stat(outpic.c_str(), &pic) == -1 ) {      // check if it exists allready, if yes, do nothing about it
             if (zip_stat_index(pzip, zipfilenum, 0, &zstat)) {
                 fprintf(stderr, "error: can't get stat for %s\n", fname);
                 return -4;
             } // are we able to find it in the zp file ?
             if ((buffer = (char *)malloc(zstat.size)) == NULL) {
                 fprintf(stderr, "error: can't get enough memory\n");
                 return -5;
             }
             if ((zfile = zip_fopen_index(pzip, zipfilenum, 0)) == NULL) {
                 fprintf(stderr, "error: can't open zip file %s\n", fname);
                 return -7;
             }
             if (zip_fread(zfile, buffer, zstat.size) == -1 ) { // fill buffer from jpg
                 fprintf(stderr, "could not extract jpg file from %s.\n", outfile.c_str());
             };
             zip_fclose(zfile);  // close file after reading it from zip

             if ( epgimagesdir != "" ) {
#ifdef USE_IMAGEMAGICK
                    if ( imgsize > 0 ) {

                         Image *image, *scaled_image;
                         ImageInfo *image_info;
                         ExceptionInfo *exception;

                         if ((exception=(ExceptionInfo *) AcquireMagickMemory(sizeof(*exception))) == NULL){
                            fprintf(stderr,"can't AcquireMagickMemory");
                            return -4;
                         }
                         GetExceptionInfo(exception);

                         image_info = CloneImageInfo((ImageInfo *) NULL);
                         image = BlobToImage(image_info,  buffer, zstat.size, exception);
                         if (exception->severity != UndefinedException)
                            CatchException(exception);

                         double factor = double(imgsize) / std::max(image->columns, image->rows);
                         scaled_image = ScaleImage(image, (int)(image->columns * factor + 0.5), (int)(image->rows * factor + 0.5), exception);
                         if (exception->severity != UndefinedException)
                            CatchException(exception);

                         strcpy(scaled_image->filename, outpic.c_str());
                         WriteImage(image_info, scaled_image);

                         DestroyImage(image);
                         DestroyImage(scaled_image);
                         DestroyImageInfo(image_info);
                         DestroyExceptionInfo(exception);
                    }
#endif
                    if ( imgsize == 0 ) {
                          // unzip unchanged .jpegs
                          FILE *fh1 = NULL;
                          if ((fh1 = fopen(outpic.c_str(), "w"))) {
                              fwrite(buffer, 1, zstat.size, fh1);
                              fclose(fh1);
                          } else {
                              fprintf(stderr, "could not write image file %s.\n", outpic.c_str());
                          }
                    }
             }
             free(buffer); buffer = NULL;
         }
     }
  } // loop through zip file for pictures


  for (zipfilenum = 0; zipfilenum < num_files; zipfilenum++) {
     if ((fname = zip_get_name(pzip, zipfilenum, 0)) == NULL) { // get the filename
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
             fprintf(stderr, "error: can't open zip file %s\n", fname);
             return -7;
         }

        // fill buffer from xml
        if (zip_fread(zfile, buffer, zstat.size) == -1 ) {
            fprintf(stderr, "could not extract xml file from %s.\n", outfile.c_str());
        }
        zip_fclose(zfile);
        // start the processing
        setlocale(LC_ALL, "");
        const char *encoding  = nl_langinfo(CODESET); // get encoding
        fprintf(stderr, "Encoding detected: %s \n", encoding );
        encoder = xmlFindCharEncodingHandler(encoding);
        outdocbuffer = xmlOutputBufferCreateFilename (outfile.c_str(), encoder, 0);
        if (outdocbuffer == NULL) {
        	  xmlOutputBufferClose(outdocbuffer);
            fprintf(stderr, "could not create file %s.\n",outfile.c_str());
        }
        writer = xmlNewTextWriter(outdocbuffer); // create the writer
        if (writer == NULL) {
            xmlFreeTextWriter(writer);
            fprintf(stderr, "could not create file output for %s.\n", outfile.c_str());
        }
        reader = xmlReaderForMemory(buffer, zstat.size,confdir.c_str() ,NULL , XML_PARSE_NOENT | XML_PARSE_DTDLOAD); // create the reader
        if (reader != NULL) { // reader created successfull
            parseretval = xmlTextReaderRead(reader);
            while (parseretval == 1) {
                processNode(reader, writer, user_data);
                parseretval = xmlTextReaderRead(reader);
            } // parsing happens here
            xmlFreeTextReader(reader); // first destroy the reader ...
            xmlFreeTextWriter(writer); // ... and writer instance
            if (parseretval != 0) { // something bad happened, cleanup
                fprintf(stderr, "failed to parse %s,\n skipping rest of the file and cleanup\n",file.c_str());
                xmlFree(user_data->primetime);        user_data->primetime = NULL ;
                xmlFree(user_data->technics_bw);      user_data->technics_bw = NULL;
                xmlFree(user_data->technics_co_channel);  user_data->technics_co_channel = NULL;
                xmlFree(user_data->technics_vt150);     user_data->technics_vt150 =NULL;
                xmlFree(user_data->technics_coded);     user_data->technics_coded = NULL;
                xmlFree(user_data->technics_blind);     user_data->technics_blind = NULL;
                xmlFree(user_data->age_marker);       user_data->age_marker = NULL;
                xmlFree(user_data->live);         user_data->live = NULL;
                user_data->tip = 0;
                xmlFree(user_data->title);          user_data->title = NULL;
                xmlFree(user_data->subtitle);       user_data->subtitle = NULL;
                xmlFree(user_data->comment_long);     user_data->comment_long = NULL;
                xmlFree(user_data->comment_middle);     user_data->comment_middle = NULL;
                xmlFree(user_data->comment_short);      user_data->comment_short = NULL;
                xmlFree(user_data->themes);         user_data->themes = NULL;
                xmlFree(user_data->sequence);       user_data->sequence = NULL;
                xmlFree(user_data->stars);          user_data->stars = NULL;
                xmlFree(user_data->attribute);        user_data->attribute = NULL;
                xmlFree(user_data->technics_stereo);    user_data->technics_stereo = NULL;
                xmlFree(user_data->technics_dolby);     user_data->technics_dolby = NULL;
                xmlFree(user_data->technics_wide);      user_data->technics_wide = NULL;
                xmlFree(user_data->country);        user_data->country = NULL;
                xmlFree(user_data->year);         user_data->year = NULL;
                xmlFree(user_data->moderator);        user_data->moderator = NULL;
                xmlFree(user_data->studio_guest);     user_data->studio_guest = NULL;
                xmlFree(user_data->regisseur);        user_data->regisseur = NULL;
                xmlFree(user_data->actor);          user_data->actor = NULL;
            }
        }
        else fprintf(stderr, "Unable to get xml\n");
	free(buffer); buffer = NULL;
    } // if xml
  } // loop through zip file for xml

  if (zip_close(pzip)) {
      fprintf(stderr, "error: can't close zip file\n");
      return -9;
  }   // close the zip
  fprintf(stderr,"Converted %s. %i epg items with %i pictures exported overall.\n", file.c_str(), importedItems.size(), imageCount);
  return 0; // ... and done !
}
