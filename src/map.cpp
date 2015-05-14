/*
 * map.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#include "map.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
using namespace std;
namespace tank {
Map::Map() {

}
Map::Map(ResourceManager *resourceManger, WarField * field, list<Object> *objects, SDL_Surface *screen) {
	this->resourceManger = resourceManger;
	this->objects = objects;
	this->field = field;
	this->screen = screen;


}
void Map::SaveObject(string fileName){
	list<xmlNodePtr> nodes;
	list<Object>::iterator ite;
	if(objects!=NULL){
	cout<<"Save object "<<objects->size()<<endl;
	for (ite=objects->begin();ite!=objects->end(); ite++) {
		xmlNodePtr node = xmlNewNode(NULL, BAD_CAST("object"));
		string name=(*ite).GetName();
//		cout<<"Save object1"<<endl;
		xmlNewProp(node, BAD_CAST("name"), BAD_CAST(name.c_str()));
		xmlNodePtr image = xmlNewNode(NULL, BAD_CAST("image"));
		xmlNodePtr pos = xmlNewNode(NULL, BAD_CAST("pos"));
		xmlNodePtr imageName = xmlNewText(BAD_CAST(name.c_str()));
		char buf[20];
//		time_t now;
//		time(&now);
//		struct tm *timenow;
//		timenow = localtime(&now);
//		struct timeval tval;
//		gettimeofday(&tval, NULL);
//		srandom(tval.tv_usec);
		SDL_Rect rect=ite->GetRect();
		sprintf(buf, "%d,%d",rect.x, rect.y);
//		cout<<buf<<endl;
		xmlNodePtr posContent = xmlNewText(BAD_CAST(buf));
		xmlAddChild(image, imageName);
		xmlAddChild(pos, posContent);
		xmlAddChild(node, image);
		xmlAddChild(node, pos);
		nodes.push_back(node);
	}
	AddNodeList(fileName,&nodes);
	}else{
		cout<<"Objects is null save none."<<endl;
	}

}

void Map::AddNodeList(string fileName,	list<xmlNodePtr> *nodes){
	doc = xmlReadFile(fileName.c_str(), "utf-8", XML_PARSE_RECOVER);
		if (doc == NULL) {
			cout << "Can not read " << fileName << " file!" << endl;
			return;
		}
		curNode = xmlDocGetRootElement(doc);
		if (curNode == NULL) {
			cout << "Empty file !" << endl;
			xmlFreeDoc(doc);
			return;
		}
		if (xmlStrcmp(curNode->name, BAD_CAST("map"))) {
			if (xmlStrcmp(curNode->name, BAD_CAST("resource"))) {
				cout << "Wrong type on root node != map or resource . " << endl;
				xmlFreeDoc(doc);
				return;
			}
		}
		list<xmlNodePtr>::iterator ite;
		for(ite=nodes->begin();ite!=nodes->end();ite++){
			xmlAddChild(curNode, (*ite));
		}
		xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", XML_PARSE_RECOVER);
		xmlCleanupParser();
		xmlFreeDoc(doc);
}
void Map::AddNode(string fileName, xmlNodePtr node) {
	doc = xmlReadFile(fileName.c_str(), "utf-8", XML_PARSE_RECOVER);
	if (doc == NULL) {
		cout << "Can not read " << fileName << " file!" << endl;
		return;
	}
	curNode = xmlDocGetRootElement(doc);
	if (curNode == NULL) {
		cout << "Empty file !" << endl;
		xmlFreeDoc(doc);
		return;
	}
	if (xmlStrcmp(curNode->name, BAD_CAST("map"))) {
		if (xmlStrcmp(curNode->name, BAD_CAST("resource"))) {
			cout << "Wrong type on root node != map or resource . " << endl;
			xmlFreeDoc(doc);
			return;
		}
	}
	xmlAddChild(curNode, node);
	xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", XML_PARSE_RECOVER);
	xmlCleanupParser();
	xmlFreeDoc(doc);

}
void Map::DeleteNode(string filename, xmlNodePtr node) {

}
void Map::ParseMap(string name) {
	doc = xmlReadFile(name.c_str(), "utf-8", XML_PARSE_RECOVER);
	if (doc == NULL) {
		cout << "Can not read " << name << " file!" << endl;
		return;
	}
	curNode = xmlDocGetRootElement(doc);
	if (curNode == NULL) {
		cout << "Empty file !" << endl;
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(curNode->name, BAD_CAST("map"))) {
		cout << "Wrong type on root node != map." << endl;
		xmlFreeDoc(doc);
		return;
	}

	curNode = curNode->children;
	xmlNodePtr propNodePtr = curNode;
	while (curNode != NULL) {

		if (curNode->type == XML_ELEMENT_NODE) {
			string id;
			string name;
			string path;

			//sound node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "sound") == 0)) {

				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {
					string resName;
					if (xmlStrcmp(n->name, (xmlChar*) "music") == 0) {
						//					cout << "path :" << xmlNodeGetContent(n) << endl;
						resName = (char*) xmlNodeGetContent(n);
					} else if (xmlStrcmp(n->name, (xmlChar*) "chunck") == 0) {
						//					cout << "path :" << xmlNodeGetContent(n) << endl;
						resName = (char*) xmlNodeGetContent(n);
					}

					n = n->next;
				}
				xmlFree(temp);

			}
			//animate node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "animate") == 0)) {

				int x, y;
				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
							//						cout << szAttr << endl;
							//						xmlFree(szAttr);
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {
					string resName;
					if (xmlStrcmp(n->name, (xmlChar*) "image") == 0) {
						resName = (char*) xmlNodeGetContent(n);
					}

					n = n->next;
				}
				xmlFree(temp);

			}
			int x = -1, y = -1;
			string resName = "";
			//object node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "object") == 0)) {
				//				cout<<"c-------------"<<endl;

				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
							//						cout << szAttr << endl;
							//						xmlFree(szAttr);
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {

					if (xmlStrcmp(n->name, (xmlChar*) "image") == 0) {
						resName = (char*) xmlNodeGetContent(n);
					} else if (xmlStrcmp(n->name, (xmlChar*) "pos") == 0) {
						//					cout << "size :" << xmlNodeGetContent(n) << endl;
						string s = (char*) xmlNodeGetContent(n);
						int pos = s.find(',');
						x = atoi(s.substr(0, pos).c_str());
						y = atoi(s.substr(pos + 1, s.length() - pos).c_str());
						//					cout << x << ", " << y << endl;
					} else if (xmlStrcmp(n->name, (xmlChar*) "tt") == 0) {
						cout << "tt :" << xmlNodeGetContent(n) << endl;
					}

					n = n->next;
				}

				xmlFree(temp);
				if (name != "" && resName != "") {

					Resource * res = resourceManger->GetResourceByName(resName);
					if (res != NULL) {
						//						cout << x << "," << y << " " << name << " resName " << resName << endl;
						Object o;
						o.SetName(name);
						o.SetSize(res->GetHeight(), res->GetWidth());
						o.SetPos(x, y);
						o.SetSurface(this->screen);
						o.SetField(field);
						o.SetType(5);
						objects->push_back(o);

					}
				}

			}

			//objects node process


		}

		curNode = curNode->next;
	}

	xmlFreeDoc(doc);
}

void Map::ParseResource(string name) {
	doc = xmlReadFile(name.c_str(), "utf-8", XML_PARSE_RECOVER);
	if (doc == NULL) {
		cout << "Can not read " << name << " file!" << endl;
		return;
	}
	curNode = xmlDocGetRootElement(doc);
	if (curNode == NULL) {
		cout << "Empty file !" << endl;
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(curNode->name, BAD_CAST("resource"))) {
		cout << "Wrong type on root node != resource." << endl;
		xmlFreeDoc(doc);
		return;
	}

	curNode = curNode->children;
	xmlNodePtr propNodePtr = curNode;
	while (curNode != NULL) {

		if (curNode->type == XML_ELEMENT_NODE) {
			Resource res;
			string name;
			string path;
			int x = -1, y = -1;
			//image node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "image") == 0)) {

				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {
					if (xmlStrcmp(n->name, (xmlChar*) "path") == 0) {
						//					cout << "path :" << xmlNodeGetContent(n) << endl;
						path = (char*) xmlNodeGetContent(n);
					} else if (xmlStrcmp(n->name, (xmlChar*) "size") == 0) {
						//					cout << "size :" << xmlNodeGetContent(n) << endl;
						string s = (char*) xmlNodeGetContent(n);
						int pos = s.find(',');
						x = atoi(s.substr(0, pos).c_str());
						y = atoi(s.substr(pos + 1, s.length() - pos).c_str());
						//						cout << x << ", " << y << endl;
					} else if (xmlStrcmp(n->name, (xmlChar*) "tt") == 0) {
						//						cout << "path :" << xmlNodeGetContent(n) << endl;
					}
					n = n->next;
				}
				xmlFree(temp);
				res.LoadImage(path);

			}
			//music node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "music") == 0)) {

				int x, y;
				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
							//						cout << szAttr << endl;
							//						xmlFree(szAttr);
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {
					if (xmlStrcmp(n->name, (xmlChar*) "path") == 0) {
						//					cout << "path :" << xmlNodeGetContent(n) << endl;
						path = (char*) xmlNodeGetContent(n);
					} else if (xmlStrcmp(n->name, (xmlChar*) "size") == 0) {
						//					cout << "size :" << xmlNodeGetContent(n) << endl;
						string s = (char*) xmlNodeGetContent(n);
						int pos = s.find(',');
						x = atoi(s.substr(0, pos).c_str());
						y = atoi(s.substr(pos + 1, s.length() - pos).c_str());
						//					cout << x << ", " << y << endl;
					} else if (xmlStrcmp(n->name, (xmlChar*) "tt") == 0) {
						cout << "path :" << xmlNodeGetContent(n) << endl;
					}
					n = n->next;
				}
				xmlFree(temp);
				res.LoadMusic(path);
			}

			//chunck node process
			if ((xmlStrcmp(curNode->name, (xmlChar *) "chunck") == 0)) {
				//				cout<<"c-------------"<<endl;
				int x, y;
				if (xmlHasProp(curNode, BAD_CAST("name"))) {
					propNodePtr = curNode;
					xmlAttrPtr attrPtr = propNodePtr->properties;
					while (attrPtr != NULL) {
						if (!xmlStrcmp(attrPtr->name, BAD_CAST("name"))) {
							name = (char*) xmlGetProp(propNodePtr, BAD_CAST("name"));
							//						cout << szAttr << endl;
							//						xmlFree(szAttr);
						}
						attrPtr = attrPtr->next;
					}
				}
				temp = xmlNodeGetContent(curNode);
				//			cout<<curNode->name<<" "<<temp<<endl;
				xmlNodePtr n = curNode->children;
				while (n != NULL) {
					if (xmlStrcmp(n->name, (xmlChar*) "path") == 0) {
						//					cout << "path :" << xmlNodeGetContent(n) << endl;
						path = (char*) xmlNodeGetContent(n);
					} else if (xmlStrcmp(n->name, (xmlChar*) "size") == 0) {
						//					cout << "size :" << xmlNodeGetContent(n) << endl;
						string s = (char*) xmlNodeGetContent(n);
						int pos = s.find(',');
						x = atoi(s.substr(0, pos).c_str());
						y = atoi(s.substr(pos + 1, s.length() - pos).c_str());
						//					cout << x << ", " << y << endl;
					} else if (xmlStrcmp(n->name, (xmlChar*) "tt") == 0) {
						cout << "path :" << xmlNodeGetContent(n) << endl;
					}
					n = n->next;
				}
				xmlFree(temp);
				res.LoadChunk(path);

			}

			//ai node process


			if (name != "" && path != "") {
				res.SetName(name);
				if (x > 0 && y > 0) {
					res.SetSize(x, y);
				}
				resourceManger->AddResource(name, res);
				//				cout << "id " << id << " name " << name << endl;
				//				cout << "path " << path << endl;
				//				cout << x << "," << y << endl;
			}

		}

		curNode = curNode->next;
	}

	xmlFreeDoc(doc);

}
void Map::SetResourceManager(ResourceManager *resourceManger) {
	this->resourceManger = resourceManger;
}
}
