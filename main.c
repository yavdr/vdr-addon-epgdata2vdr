#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <libxml/parser.h>

void startElement(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
//  printf("StartElement: %s\n", name);
  if (!strcmp(name, "d21"))
  {
    *(int *)user_data = 1;
  }
}

void endElement(void *user_data, const xmlChar *name)
{
//  printf("endElement: %s\n", name);
  if (!strcmp(name, "d21"))
  {
    *(int *)user_data = 0;
  }
}

void characters(void *user_data, const xmlChar *ch, int len)
{
  
  if (*(int *)user_data == 1)
  {
    printf("characters: %d ", len);
    fwrite(ch, 1, len, stdout);
    printf("\n");
  }
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
  int user_data = 0;

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
      xmlSAXUserParseMemory(sax, &user_data, buffer, len);
      
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
