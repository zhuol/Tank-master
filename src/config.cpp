/*
 * config.cpp
 *
 *  Created on: Sep 17, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#include "config.h"

namespace tank {

Config::Config() {

}
void Config::Print() {

}
void Config::PrintMap() {
	print_xml_file("map.xml");
}
void Config::PrintResource() {
	print_xml_file("res.xml");
}
void Config::PrintObject() {
	print_xml_file("obj.xml");
}
void Config::LoadResource(ResourceManager *resourceManger) {

}

void Config::LoadMap(list<Object> * objects) {

}

void print_xml_file(string xmlFileName) {
	xmlDocPtr doc = xmlParseFile(xmlFileName.c_str());
	xmlNodePtr node;
	if (doc != NULL) {
		node = xmlDocGetRootElement(doc);
		if (node != NULL) {
			char * name;
			char* value;
			node = node->children; //get sub node
			while (node != NULL) {
				name = (char*) (node->name);
				value = (char*) xmlNodeGetContent(node);
				if (node->type == XML_ELEMENT_NODE)
				cout << name << " " << value << endl;
				xmlFree(value);
				node = node->next;
			}
			xmlFreeDoc(doc);
			xmlCleanupParser();
		} else {
			cout << "node is null." << endl;
		}
	} else {
		cout << "pase error." << endl;
	}
}

}
