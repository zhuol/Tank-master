/*
 * resource.cpp
 *
 *  Created on: Aug 27, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#include "resource.h"
#include <iostream>
using namespace std;
namespace tank {

extern SDL_Surface *load_image(string filename);

ResourceManager::ResourceManager() {

}
void ResourceManager::FreeAllResource() {
	map<string, Resource>::iterator ite;
	for (ite = resources.begin(); ite != resources.end(); ite++) {
		ite->second.FreeResource();
	}
}
Resource * ResourceManager::GetResourceByName(string name) {
	map<string, Resource>::iterator find = resources.find(name);
	if (find == resources.end()) {
		cout << "Find resource (" << name << ") failed." << endl;
		return NULL;
	} else {
		return find->second.GetResource();
	}

}
SDL_Surface * ResourceManager::GetResourceSurfaceByName(string name) {
	map<string, Resource>::iterator find = resources.find(name);
	if (find == resources.end()) {
		cout << "Find resource surface failed." << endl;
		return NULL;
	} else {
		return find->second.GetSurface();
	}
}
Mix_Music* ResourceManager::GetResourceMusicByName(string name) {
	map<string, Resource>::iterator find = resources.find(name);
	if (find == resources.end()) {
		std::cout << "Not find (" << name << ") music." << endl;
		return NULL;
	} else {
		return find->second.GetMusic();
	}
}
Mix_Chunk * ResourceManager::GetResourceChunkByName(string name) {
	map<string, Resource>::iterator find = resources.find(name);
	if (find == resources.end()) {
		return NULL;
	} else {
		return find->second.GetChunk();
	}
}
void ResourceManager::AddResource(string name, Resource resource) {
	resources[name] = resource;
}
int ResourceManager::GetSize() {
	return this->resources.size();
}
void ResourceManager::PrintAllInfo() {
	map<string, Resource>::iterator ite;
	for (ite = resources.begin(); ite != resources.end(); ite++) {
		ite->second.PrintInfo();
		cout << endl;
	}
}

Resource::Resource() {
	this->surface = NULL;
	this->chunk = NULL;
	this->music = NULL;
	this->id = -1;
	this->name = "";
	this->width = 0;
	this->height = 0;
}
Resource * Resource::GetResource() {
	return this;
}
SDL_Surface * Resource::GetSurface() {
	return this->surface;
}
void Resource::SetId(int id) {
	this->id = id;
}
int Resource::GetId() {
	return this->id;
}
void Resource::SetSize(int w, int h) {
	this->width = w;
	this->height = h;
}

int Resource::GetHeight() {
	return this->height;
}
int Resource::GetWidth() {
	return this->width;
}
void Resource::PrintInfo() {
	cout << "id:" << this->id << endl;
	cout << "name:" << this->name << endl;
	cout << "height:" << this->height << endl;
	cout << "width:" << this->width << endl;

}
void Resource::Zoom(double zoomx, double zoomy, int smooth) {
	if (surface != NULL) {
		SDL_Surface *zoomPic=NULL;
		zoomPic=zoomSurface(this->surface, zoomx, zoomy, smooth);
		if(zoomPic!=NULL){
			this->surface=zoomPic;
		}
	} else {
		cout << "zoom failed surface is null" << endl;
	}
}
void Resource::SetName(string name) {
	this->name = name;
}
string Resource::GetName() {
	return this->name;
}
void Resource::SetResource(SDL_Surface *resource) {
	this->surface = resource;
}
void Resource::Load() {
}
void Resource::LoadImage(string fileName) {
	string f;
	surface = tank::load_image(fileName);
}
void Resource::LoadChunk(string fileName) {
	this->chunk = Mix_LoadWAV(fileName.c_str());
}
void Resource::LoadMusic(string fileName) {
	this->music = Mix_LoadMUS(fileName.c_str());
}
Mix_Music * Resource::GetMusic() {
	return this->music;
}
Mix_Chunk * Resource::GetChunk() {
	return this->chunk;
}
void Resource::FreeResource() {
	if (chunk != NULL) {
		Mix_FreeChunk(chunk);
	}
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	if (surface != NULL) {
		SDL_FreeSurface(surface);
	}
}

}
