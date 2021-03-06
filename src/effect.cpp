/*
 * efect.cpp
 *
 *  Created on: Sep 8, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#include "effect.h"
#include <iostream>
using namespace std;

namespace tank {
Effect::Effect() {

	life = 1;
	x = 0;
	y = 0;
	type = -1;
	rect.x = 0;
	rect.y = 0;
	rect.h = 300;
	rect.w = 300;
	step = 0;
	alpha = SDL_ALPHA_OPAQUE;
	maxlife = 100;
	this->surface = NULL;
}
void Effect::BulletBomb() {
	char name[20];
	sprintf(name, "bulletbomb%d", life);
	//	cout << name << endl;
	SDL_Surface* res = field->GetResourceManager()->GetResourceSurfaceByName(name);
	if (res != NULL && surface != NULL) {
		//		 SDL_FillRect(surface, NULL, 0);
		SDL_SetColorKey(res, SDL_SRCCOLORKEY, 0xffffff);
		SDL_SetAlpha(res, SDL_SRCALPHA, this->alpha);
		apply_surface(x, y, res, surface, &rect);
		//		SDL_UpdateRect(surface, x, y, rect.w, rect.h);

	} else {

	}
}
void Effect::TankBomb() {
	char name[20];
	sprintf(name, "tankbomb%02d", life);
	//	cout<<name<<endl;
	SDL_Surface* res = field->GetResourceManager()->GetResourceSurfaceByName(name);
	if (res != NULL && surface != NULL) {
		//		 SDL_FillRect(surface, NULL, 0);
		SDL_SetColorKey(res, SDL_SRCCOLORKEY, 0xffffff);
		SDL_SetAlpha(res, SDL_SRCALPHA, this->alpha);
		apply_surface(x, y, res, surface, &rect);
		//		SDL_UpdateRect(surface, x, y, rect.w, rect.h);

	} else {
		cout << "tank bomb" << life << " res is null." << endl;
		//		cout<<"maxlife"<<maxlife<<endl;
	}

}

void Effect::SetField(WarField *field) {
	this->field = field;
	if (this->field == NULL)
		cout << "fffffis .";
}
void Effect::SetPos(int x, int y) {
	this->x = x;
	this->y = y;
}
void Effect::SetSurface(SDL_Surface * surface) {
	this->surface = surface;
}
void Effect::SetLife(int life) {
	this->life = life;
}
void Effect::Draw() {

	if ((life >= 1) && (life <= maxlife)) {
		//		cout << "l:" << life << endl;
		step++;
		if (step >= 1) {
			step = 0;

			switch (type) {
			case 0:
				TankBomb();
				break;
			case 1:
				BulletBomb();
				break;
			case 2:
				TankBombSound();
				break;
			case 3:
				BulletBombSound();
			case 4:
				ShootSound();
			default:
				break;
			}
			life++;
			//			cout << "life:" << life << " tanke bomb" << endl;
		}
	} else {
		life = -10;
	}
}
void Effect::SetType(int type) {
	this->type = type;
}
int Effect::GetLife() {
	return this->life;
}
int Effect::GetMaxLife() {
	return this->maxlife;
}
void Effect::SetMaxLife(int maxlife) {
	this->maxlife = maxlife;

}
void Effect::TankBombSound() {
	Mix_Chunk* chunk = this->field->GetResourceManager()->GetResourceChunkByName("tankbomb");
	if (chunk != NULL) {
		//		cout << "chunk play begin" << endl;
		chunk->volume = 60 + rand() % 10;
		if (Mix_PlayChannel( -1, chunk, 0 ) == -1) {
			//			cout << "chunk play return" << endl;
			return;
		}

	} else {
		cout << "tankbomb chunk is null." << endl;
	}
}
void Effect::BulletBombSound() {
	Mix_Chunk* chunk = this->field->GetResourceManager()->GetResourceChunkByName("bulletbomb");
	if (chunk != NULL) {
//				cout << ""Bullet bomb chunk play begin" << endl;
		chunk->volume = 15 + rand() % 10;
		if (Mix_PlayChannel( rand()%2, chunk, 0 ) == -1) {
			cout << "Bullet bomb chunk play return" << endl;
			return;
		}

	} else {
		cout << "tankbomb chunk is null." << endl;
	}
}
void Effect::ShootSound() {
	Mix_Chunk* chunk = this->field->GetResourceManager()->GetResourceChunkByName("shoot");
	if (chunk != NULL) {
		//		cout << "chunk play begin" << endl;
		chunk->volume = 20 + rand() % 10;
		if (Mix_PlayChannel( rand()%2, chunk, 0 ) == -1) {
			cout << "Shoot chunk play return" << endl;
			return;
		}

	} else {
		cout << "Shoot chunk is null." << endl;
	}
}

}
