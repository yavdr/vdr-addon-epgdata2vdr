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
	
	// read categories and genre into ONE map. They don't share id's (Logic category div 100 = genre, category is never full 100)
	
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

int cDataMap::processData(xmlTextReaderPtr reader) 
{
	// element callback from read_xmlfile
	// args:	pointer to the xmlTextreader
	string value;
	int epgdataid;
	int retval;
	
	// get name, type and depth in the xml structure
    string name = string((char *)xmlTextReaderConstName(reader));
	int type = xmlTextReaderNodeType(reader) ;
	int depth = xmlTextReaderDepth(reader) ;


	// get ca0/ca1 or g0/g1 depending which file we read
	if (type == XML_READER_TYPE_ELEMENT && depth == 2 && (name.compare("ca0")||name.compare("g0"))) {
		epgdataid = atol((char *)xmlXPathCastNodeToString(xmlTextReaderExpand(reader))); 
		retval = xmlTextReaderNext(reader); // jump to end element 
		retval = xmlTextReaderNext(reader); // jump to next start element 
		value = string((char *)xmlXPathCastNodeToString(xmlTextReaderExpand(reader)));
		datamap[epgdataid]  = value;
	}
	return 0 ; 
}

string cDataMap::GetStr(int dataid)
{
	return datamap[dataid];
}



