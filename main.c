#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zip.h>
#include <libxml/parser.h>

int longest = 0;

typedef struct {
  xmlChar *buffer;
  int len;
  int index;
  int state;

  int broadcast_id;
  int tvshow_id;
  int tvchannel_id;
  xmlChar *title;
  xmlChar *comment_short;
  xmlChar *comment_long;
  time_t starttime;
  time_t vps;
  int tvshow_length;
} UserData, * UserDataPtr;

static xmlEntity xmlEntityauml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "auml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "ä", BAD_CAST "ä", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityAuml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "Auml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "Ä", BAD_CAST "Ä", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityouml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "ouml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "ö", BAD_CAST "ö", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityOuml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "Ouml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "Ö", BAD_CAST "Ö", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityuuml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "uuml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "ü", BAD_CAST "ü", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityUuml = {
    NULL, XML_ENTITY_DECL, BAD_CAST "Uuml",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "Ü", BAD_CAST "Ü", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

static xmlEntity xmlEntityszlig = {
    NULL, XML_ENTITY_DECL, BAD_CAST "szlig",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "ß", BAD_CAST "ß", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0, 1
};

void startElement(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
  UserDataPtr pud = (UserDataPtr)user_data;

//  printf("StartElement: %s\n", name);
  if (!strcmp(name, "d0") ||
      !strcmp(name, "d1") ||
      !strcmp(name, "d2") ||
      !strcmp(name, "d4") ||
      !strcmp(name, "d7") ||
      !strcmp(name, "d8") ||
      !strcmp(name, "d19") ||
      !strcmp(name, "d21") ||
      !strcmp(name, "d23"))
  {
    pud->state = 1;
  }
}

void endElement(void *user_data, const xmlChar *name)
{
  struct tm tm;

  UserDataPtr pud = (UserDataPtr)user_data;

//  printf("endElement: %s\n", name);
  pud->state = 0;
  pud->buffer[pud->index] = '\0';
  if (!strcmp(name, "d0"))
  {
    pud->broadcast_id = atol(pud->buffer);
  }
  if (!strcmp(name, "d1"))
  {
    pud->tvshow_id = atol(pud->buffer);
  }
  else if (!strcmp(name, "d2"))
  {
    pud->tvchannel_id = atol(pud->buffer);
  }
  else if (!strcmp(name, "d4"))
  {
    sscanf(pud->buffer, "%04d-%02d-%02d %02d:%02d:%02d", &tm.tm_year, 
     &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1; 
    tm.tm_isdst = -1;
    pud->starttime = mktime(&tm);
    pud->starttime -= tm.tm_gmtoff;
  }
  else if (!strcmp(name, "d7"))
  {
    pud->tvshow_length = atol(pud->buffer) * 60;
  }
  else if (!strcmp(name, "d8"))
  {
    if (strlen(pud->buffer) == 5)
    {
      gmtime_r(&pud->starttime, &tm);
      sscanf(pud->buffer, "%02d:%02d", &tm.tm_hour, &tm.tm_min);
      tm.tm_sec = 0;
      pud->vps = mktime(&tm);
      pud->vps -= tm.tm_gmtoff;
    }
    else
    {
      pud->vps = 0;
    }
  }
  else if (!strcmp(name, "d19"))
  {
    pud->title = strdup(pud->buffer);
  }
  else if (!strcmp(name, "d21"))
  {
    pud->comment_long = strdup(pud->buffer);
  }
  else if (!strcmp(name, "d23"))
  {
    pud->comment_short = strdup(pud->buffer);
  }
  else if (!strcmp(name, "data"))
  {
    printf("C %d\n", pud->tvchannel_id);
    printf("E %d %d %d 50\n", pud->broadcast_id, pud->starttime, pud->tvshow_length);
    printf("T %s\n", pud->title);
    printf("S %s\n", pud->comment_short);
    printf("D %s\n", pud->comment_long);
    if (pud->vps)
    {
      printf("V %d\n", pud->vps);
    }
    printf("e\n");
    printf("c\n");

//    printf("tvshow_id: %d\n", pud->tvshow_id);
    free(pud->title);
    pud->title = NULL;
    free(pud->comment_long);
    pud->comment_long = NULL;
    free(pud->comment_short);
    pud->comment_short = NULL;
  }
  pud->index = 0;
}

void characters(void *user_data, const xmlChar *ch, int len)
{
  UserDataPtr pud = (UserDataPtr)user_data;
  
  if (pud->state == 1)
  {
    if (len + 1 > (pud->len - pud->index))
    {
       pud->len += len + 1;

      if ((pud->buffer = (xmlChar *)realloc(pud->buffer, pud->len)) == NULL)
      {
        fprintf(stderr, "error: can't get enough memory\n");
        exit(-1);
      }
    }
    memcpy(&pud->buffer[pud->index], ch, len);
    pud->index += len;
  }
}

static xmlEntityPtr
my_getEntity(void *user_data, const xmlChar *name) {
//    printf("entity: %s\n", name);
    if (!strcmp(name, "auml"))
    {
      return &xmlEntityauml;
    }
    else if (!strcmp(name, "Auml"))
    {
      return &xmlEntityAuml;
    }
    else if (!strcmp(name, "ouml"))
    {
      return &xmlEntityouml;
    }
    else if (!strcmp(name, "Ouml"))
    {
      return &xmlEntityOuml;
    }
    else if (!strcmp(name, "uuml"))
    {
      return &xmlEntityuuml;
    }
    else if (!strcmp(name, "Uuml"))
    {
      return &xmlEntityUuml;
    }
    else if (!strcmp(name, "szlig"))
    {
      return &xmlEntityszlig;
    }
    return xmlGetPredefinedEntity(name);
}

int main(int argc, char *argv[])
{
  struct zip *pzip;
  int num_files;
  int i;
  const char *fname;
  struct zip_stat zstat;
  FILE *fhout;
  struct zip_file *zfile;
  char *buffer;
  int len;
  xmlSAXHandlerPtr sax;
  UserData user_data;  
  user_data.state = 0;
  user_data.index = 0;

  user_data.len = 10240;

  if ((user_data.buffer = (xmlChar *)calloc(1, user_data.len)) == NULL)
  {
    fprintf(stderr, "error: can't get enough memory\n");
    return -1;
  }
    
  if (argc != 2)
  {
    fprintf(stderr, "error: invalid number of arguments\n");
    return -1;
  }

  if ((pzip = zip_open(argv[1], 0, NULL)) == NULL)
  {
    fprintf(stderr, "error: can't open %s\n", argv[1]);
    return -2;
  }
  
  num_files = zip_get_num_files(pzip);
 
  for (i = 0; i < num_files; i++)
  {
    if ((fname = zip_get_name(pzip, i, 0)) == NULL)
    {
      fprintf(stderr, "error: can't get filename for index %d\n", i);
      return -3;
    } 
    if (!strcmp(fname + strlen(fname) - 4, ".xml"))
    {
//      printf("Filename: %s\n", fname);
      if (zip_stat_index(pzip, i, 0, &zstat))
      {
        fprintf(stderr, "error: can't get stat for %s\n", fname);
        return -4;
      }
      if ((buffer = (char *)malloc(zstat.size)) == NULL)
      {
        fprintf(stderr, "error: can't get enough memory\n");
        return -5;
      }
#if 0
      if ((fhout = fopen(fname, "w")) == NULL)
      {
        fprintf(stderr, "error: can't can't open file %s\n", fname);
        return -6;
      }
#endif
      if ((zfile = zip_fopen_index(pzip, i, 0)) == NULL)
      {
        fprintf(stderr, "error: can't can't open zip file %s\n", fname);
        return -7;
      }
 
      len = zip_fread(zfile, buffer, zstat.size);
      //fwrite(buffer, 1, len, fhout);
      if ((sax = (xmlSAXHandlerPtr)calloc(1, sizeof(xmlSAXHandler))) == NULL)
      {
        fprintf(stderr, "error: can't getenough memory for xmlSAXHandler\n");
        return -8;
      }
      sax->startElement = startElement;
      sax->endElement = endElement;
      sax->characters = characters; 
      sax->getEntity = my_getEntity;
      xmlSAXUserParseMemory(sax, &user_data, buffer, len);
      printf("longest: %d\n", longest);      

      free(sax);
      zip_fclose(zfile);
#if 0
      fclose(fhout);
#endif
    }
  }

  if (zip_close(pzip))
  {
    fprintf(stderr, "error: can't close zip file\n");
    return -9;
  }   

  return 0;
}
