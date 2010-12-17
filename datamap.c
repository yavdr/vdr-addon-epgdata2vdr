/*
 * datamap.c: epgdata2vdr epgdata.com parser for vdr
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "datamap.h"


cDataMap::cDataMap (string confdir)
{
	datamap.clear();
	read_xml_file(confdir);
}

cDataMap::~cDataMap ()
{
	datamap.clear();
}


int cDataMap::read_xml_file(string confdir)
{
	xmlTextReaderPtr reader;
    int ret;
	string genre = confdir + "genre.xml" ;
    string category = confdir + "category.xml" ;

	// read categories and genre into ONE map. They don't share id's (Logic genre div 100 = category, genre is never full 100)

	// categories
    reader = xmlReaderForFile(category.c_str(), "iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processData(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);

        if (ret != 0) {
			fprintf(stderr, "epgdata2vdr : failed to parse %s\n", category.c_str() );
			return ret;
		}
    }
	else {
		fprintf(stderr, "Unable to open %s\n", category.c_str());
		return -1;
	}

	// genres
	reader = xmlReaderForFile(genre.c_str(), "iso-8859-1" , XML_PARSE_NOENT | XML_PARSE_DTDLOAD);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processData(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
			fprintf(stderr, "Failed to parse %s.\n", genre.c_str());
			return ret;
		}
    }
	else {
		fprintf(stderr, "Unable to open %s\n", genre.c_str());
		return -1;
	}
	return 0;
}

int cDataMap::processData(xmlTextReaderPtr reader)
{
	// element callback from read_xmlfile
	// args:	pointer to the xmlTextreader
	xmlNodePtr node ;
	xmlChar *content;
	string value;
	int epgdataid;
	int retval;

	// get name, type and depth in the xml structure
        string name = string((char *)xmlTextReaderConstName(reader));

	int type = xmlTextReaderNodeType(reader) ;
	int depth = xmlTextReaderDepth(reader) ;


	// get ca0/ca1 or g0/g1 depending which file we read
	if (type == XML_READER_TYPE_ELEMENT && depth == 2 && (name.compare("ca0") == 0)) {
		node = xmlTextReaderExpand(reader);
		content = xmlXPathCastNodeToString(node);
		node = NULL ;
		epgdataid = atoi((char *)content);
		xmlFree(content);

		retval = xmlTextReaderNext(reader); // jump to end element
		retval = xmlTextReaderNext(reader); // jump to next start element

		node = xmlTextReaderExpand(reader);
		content = xmlXPathCastNodeToString(node);
		node = NULL ;
		value = string((char *)content);
		xmlFree(content);
		datamap[epgdataid]  = value;

	} else if (type == XML_READER_TYPE_ELEMENT && depth == 2 && ( name.compare("g0") == 0 )) {
		node = xmlTextReaderExpand(reader);
		content = xmlXPathCastNodeToString(node);
		node = NULL ;
		epgdataid = atoi((char *)content);
		xmlFree(content);

		retval = xmlTextReaderNext(reader); // jump to end element
		retval = xmlTextReaderNext(reader); // jump to next start element

		node = xmlTextReaderExpand(reader);
		content = xmlXPathCastNodeToString(node);
		node = NULL ;
		value = string((char *)content);
		xmlFree(content);
		datamap[epgdataid]  = value;
	}
	return 0 ;
}

string cDataMap::GetStr(int dataid)
{
        return datamap[dataid];
}



