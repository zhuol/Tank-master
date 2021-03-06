/*
 * resource.h
 *
 *  Created on: Aug 27, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

namespace tank {

class Resource {
private:
	SDL_Surface *surface;
	int width,height;
	string name;
	Mix_Music *music;
	Mix_Chunk *chunk;
	int id;
public:
	Resource();
	SDL_Surface * GetSurface();
	Resource * GetResource();
	void SetId(int id);
	int GetId();
	void SetResource(SDL_Surface *resource);
	void Load();
	void LoadImage(string fileName);
	void LoadMusic(string fileName);
	void LoadChunk(string fileName);
	Mix_Music* GetMusic();
	Mix_Chunk * GetChunk();
	void FreeResource();
	void SetSize(int w,int h);
	int GetHeight();
	int GetWidth();
	void PrintInfo();
	void SetName(string name);
	string GetName();
	void Zoom( double zoomx, double zoomy, int smooth);

};

class ResourceManager {
private:
	map < string, Resource > resources;
public:
	ResourceManager();
	void Load();
	void AddResource(string name,Resource);
	SDL_Surface * GetResourceById(int Id);
	Resource * GetResourceByName(string name);
	SDL_Surface * GetResourceSurfaceByName(string name);
	Mix_Music* GetResourceMusicByName(string name);
	Mix_Chunk * GetResourceChunkByName(string name);
	int GetSize();
	void FreeAllResource();
	void PrintAllInfo();
};

}

#endif /* RESOURCE_H_ */
