/*
 * datamap.c: TVM2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "datamap.h"


cDataMap::cDataMap ()
{
	datamap.clear();
	read_xml_file();
}

cDataMap::~cDataMap ()
{
	datamap.clear();
}


int cDataMap::read_xml_file()
{
	xmlTextReaderPtr reader;
    int ret;
	
	// categories 
    reader = xmlReaderForFile("category.xml", "iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
    if (reader != NULL) 
	{
        ret = xmlTextReaderRead(reader);
        while (ret == 1) 
		{
            processData(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) fprintf(stderr, "category.xml : failed to parse\n");
    } 
	else fprintf(stderr, "Unable to open category.xml\n");
	// genres
	reader = xmlReaderForFile("genre.xml", "iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
    if (reader != NULL) 
	{
        ret = xmlTextReaderRead(reader);
        while (ret == 1) 
		{
            processData(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) fprintf(stderr, "Failed to parse genre.xml\n");
    } 
	else fprintf(stderr, "Unable to open genre.xml\n");
}

void cDataMap::processData(xmlTextReaderPtr reader) 
{
	// element callback from read_xmlfile
	// args:	pointer to the xmlTextreader
	xmlChar *value;
    char *name = (char *)xmlTextReaderConstName(reader);
	int type = xmlTextReaderNodeType(reader) ;
	int depth = xmlTextReaderDepth(reader) ;
	int retval;
	int epgdataid;

	if (type == XML_READER_TYPE_ELEMENT && depth == 2 && (!strcmp( name,"ca0")||!strcmp( name,"g0"))) {
		epgdataid = atol((char *)xmlXPathCastNodeToString(xmlTextReaderExpand(reader))); 
		retval = xmlTextReaderNext(reader); // end element 
		retval = xmlTextReaderNext(reader); // start element next
		value = xmlXPathCastNodeToString(xmlTextReaderExpand(reader));
		datamap[epgdataid]  = (char *)value;
	}
}

char* cDataMap::GetStr(int dataid)
{
	return datamap[dataid];
}



