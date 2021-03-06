/*
 * game.cpp
 *
 *  Created on: Aug 27, 2011
 *      Author: cty
 *  Copyright (c) 2014 evilbinary.org. All rights reserved.
 *     rootntsd@gmail.com
 */

#include "game.h"
#include <iostream>
#include "map.h"
using namespace std;

namespace tank {

bool Game::quit = false;
SDL_Surface * Game::screen = NULL;
SDL_Thread * Game::thread = NULL;
SDL_Surface * Game::background = NULL;
SDL_Surface * Game::front = NULL;
SDL_Event Game::event;

list<WarField> Game::warfields;
list<Tank> Game::tanks;
list<Bullet> Game::bullets;
ResourceManager Game::resourceManager;
list<Effect> Game::effects;
list<Object> Game::objects;

Game::Game() {
	quit = false;
	textColor.r = 0;
	textColor.g = 0xff;
	textColor.b = 0;

	tankdeadnum = 0;
	bulletdeadnum = 0;
	direct = -1;
	shoot = -1;
	fps = 50;
	bombnum = 0;
	music = NULL;
	keycount = 0;
	candraw = 0;
	lpress = 0;
	objnum = 0;
	screenwidth = SCREEN_WIDTH;
	screenheight = SCREEN_HEIGHT;
}
void Game::Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		cout << "init failed" << endl;
		return;
	}
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Tank", NULL );
	if (screen == NULL) {
		cout << "Vedio failed." << endl;
		return;
	}
	if (TTF_Init() == -1) {
		cout << "Font failed." << endl;
		return;
	}
	this->font = TTF_OpenFont("lazy.ttf", 18);

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
		cout << "Sound failed." << endl;
		return;
	}

	if (SDL_Flip(screen) == -1) {

		return;
	}

	WarField w0;
	w0.Init();
	w0.SetResourceManager(&resourceManager);
	w0.SetSurface(screen);
	warfields.push_back(w0);
	Map* m = new Map(&resourceManager, &*warfields.begin(), &objects, screen);
	this->map = m;
	map->ParseResource("res.xml");
	//	resourceManager.PrintAllInfo();
	map->ParseMap("map.xml");

	music = resourceManager.GetResourceMusicByName("bgmusic");
	chunk = resourceManager.GetResourceChunkByName("tankbomb");
	if (chunk != NULL)
		chunk->volume = 25;

	for (int i = 0; i < 0; i++) {
		Tank t;
		t.SetBullets(&bullets);
		t.RandPos();
		t.SetSurface(this->screen);
		t.SetField(&*warfields.begin());

		string name;
		switch (rand() % 3) {
		case 0:
			name = "tank01";
			t.SetType(1000 + i);
			t.SetId(1000 + i);
			t.SetColor(0xff0000);
			break;
		case 1:
			name = "tank02";
			t.SetType(2000 + i);
			t.SetId(2000 + i);
			break;
		case 2:
			t.SetType(3000 + i);
			t.SetId(3000 + i);
			name = "tank03";
			break;
		}
		t.SetName(name);
		this->tanks.push_back(t);
	}
	for (int i = 0; i < 0; i++) {
		Object o;
		o.SetName("block");
		o.SetId(2);
		o.RandPos();
		o.SetSize(16, 16);
		o.SetSurface(this->screen);
		o.SetField(&*warfields.begin());
		o.SetType(5);
		objects.push_back(o);
	}
	//tanks.begin()->SetType(6);
	//	this->background = resourceManager.GetResourceByName("bg");
	this->alpha = SDL_ALPHA_OPAQUE;
	//	cout<<"res size:"<<resourceManager.GetSize()<<endl;
	//	cout << "tanks:" << tanks.size() << endl;

	Resource * res = resourceManager.GetResourceByName("bg");
	if (res != NULL) {
		if (res->GetHeight() < this->screenheight || res->GetWidth() < this->screenwidth) {
			float fy = (float) res->GetHeight() / (float) this->screenheight;
			float fx = (float) res->GetWidth() / (float) this->screenwidth;
			//		cout<<fx<<",,"<<fy<<" "<<res->GetHeight()<<","<<res->GetWidth()<<endl;
			res->Zoom(2.1 - fx, 2.1 - fy, SMOOTHING_ON );
		}
	} else {
		cout << "Res bg is null." << endl;
	}

}

void Game::Start() {

	//thread = SDL_CreateThread(Game::DrawGame, NULL );
	if (Mix_PlayingMusic() == 0) {
		//Play the music
		if (Mix_PlayMusic(music, -1) == -1) {
			return;
		}
	}
}

void Game::Render() {
	tankdeadnum = 0;
	bulletdeadnum = 0;
	bombnum = 0;

	//SDL_SetAlpha(background, SDL_SRCALPHA, this->alpha);
	list<WarField>::iterator war;
	for (war = warfields.begin(); war != warfields.end(); war++) {
		//				war->Draw();
		//		war->DrawRect();
		//				war->DrawGrid();
	}
	//
	Resource * res = resourceManager.GetResourceByName("bg");
	if (res != NULL) {
		apply_surface(0, 0, res->GetSurface(), screen, NULL);
	} else {
		cout << "Res bg is null." << endl;
	}
	list<Object>::iterator obj;
	for (obj = objects.begin(); obj != objects.end(); obj++) {
		obj->Draw();
		//		obj->DrawRect();

	}

	list<Tank>::iterator tank;
	for (tank = tanks.begin(); tank != tanks.end();) {
		if (tank == tanks.begin()) {

		} else {
			tank->RandRun();
			tank->RandShoot();

		}

		if (tank->GetLife() <= 0 && tank->GetStep() == 0) {
			tankdeadnum++;
			tanks.erase(tank++);
		} else {
			tank->SetAlpha(alpha);
			tank->Draw();
			//			tank->DrawRect();
			tank++;
		}
	}
	list<Bullet>::iterator bullet;
	for (bullet = bullets.begin(); bullet != bullets.end();) {

		//			bullet->DrawRect();
		if (bullet->GetLife() <= 0 || bullet->GetStep() == 0) {
			bulletdeadnum++;
			bullets.erase(bullet++);
		} else {
			bullet->Draw();
			bullet++;
		}
	}

	list<Effect>::iterator effect;
	for (effect = effects.begin(); effect != effects.end(); effect++) {
		effect->Draw();
		if (effect->GetLife() > 0)
			bombnum++;
	}

	this->ShowState();

	if (SDL_Flip(screen) == -1) {
		return;
	}

}
void Game::Loop() {

	while (!quit) {
		this->HandleInput(); //event
		this->Logic(); //logic
		this->Render(); //render

	}
}
void Game::Logic() {

	switch (direct) {
	case 0:
		tanks.begin()->MoveLeft();
		break;
	case 1:

		break;
	case 2:
		tanks.begin()->MoveUp();
		break;
	case 3:

		break;
	case 4:
		tanks.begin()->MoveRight();
		break;
	case 5:

		break;
	case 6:
		tanks.begin()->MoveDown();
		break;
	case 7:

		break;

	default:

		break;
	}
	switch (shoot) {
	case 0:
		if (keycount >= 5) {
			tanks.begin()->Shoot();
			keycount = 0;
		}
		keycount++;
		break;
	case 1:
		break;
	default:
		break;
	}
	fps = calFrequency();
	SDL_Delay(50);

}
void Game::HandleInput() {

	while (SDL_PollEvent(&event)) {
		Effect e;
		switch (event.type) {
		case SDL_ACTIVEEVENT:

			if (event.active.state & SDL_APPACTIVE) {
				if (event.active.gain) {
					printf("App activated\n");
				} else {
					printf("App iconified\n");
				}
			}

			break;
		case SDL_MOUSEMOTION:
			if (candraw == 1 && lpress == 1) {
				Object o;
				int id = 0;
				string name = "";
				switch (objnum) {
				case 0:
					name = "block";
					id = 0;
					break;
				case 1:
					id = 1;
					name = "green";
					break;
				case 2:
					id = 2;
					name = "road";
					break;
				case 3:
					id = 3;
					name = "tree";
					break;
				case 4:
					break;
				default:
					break;
				}
				o.SetName(name);
				//				cout<<"name is:"<<name<<endl;
				o.SetId(id);
				int w, h;
				w = resourceManager.GetResourceByName(name)->GetWidth();
				h = resourceManager.GetResourceByName(name)->GetHeight();
				//				o.SetPos(((event.motion.x) / w) * w, ((event.motion.y) / h) * h);
				o.SetPos(((event.motion.x) / 16) * 16, ((event.motion.y) / 16) * 16);
				//				cout << "x:" << ((event.motion.x) / 16) * 16 << " y:"
				//						<< ((event.motion.y) / 16) * 16 << endl;
				o.SetSize(w, h);
				o.SetSurface(this->screen);
				o.SetField(&*warfields.begin());
				o.SetType(5);
				//				o.PrintPos();
				objects.push_back(o);

			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				//				warfields.begin()->SetMap(event.motion.x / MAPSIZE,
				//						event.motion.y / MAPSIZE, 5);
				//warfields.begin()->FillCell(tanks.begin()->GetRect());
				//				cout << "L[" << event.motion.y / MAPSIZE << ","
				//						<< event.motion.x / MAPSIZE << "]" << endl;


				//				e.SetField(&*(this->warfields.begin()));
				//				e.SetSurface(this->screen);
				//				e.SetPos(event.motion.x - 150, event.motion.y - 150);
				//				e.SetType(0);
				//				e.SetMaxLife(20);
				//				e.TankBombSound();
				//				//e.Draw();
				//				effects.push_back(e);
				if (candraw == 1) {
					Object o;
					int id = 0;
					string name = "";
					switch (objnum) {
					case 0:
						name = "block";
						id = 0;
						break;
					case 1:
						id = 1;
						name = "green";
						break;
					case 2:
						id = 2;
						name = "road";
						break;
					case 3:
						id = 3;
						name = "tree";
						break;
					case 4:
						break;
					default:
						break;
					}
					o.SetName(name);
					o.SetId(id);
					int w, h;
					w = resourceManager.GetResourceByName(name)->GetWidth();
					h = resourceManager.GetResourceByName(name)->GetHeight();
					o.SetPos(((event.motion.x) / 16) * 16, ((event.motion.y) / 16) * 16);
					//					o.SetPos(((event.motion.x) / w) * w, ((event.motion.y) / h) * h);
					//					cout<<"w"<<w<<" h"<<h<<endl;
					//					cout << "x:" << ((event.motion.x) / 16) * 16 << " y:"
					//							<< ((event.motion.y) / 16) * 16 << endl;
					o.SetSize(w, h);
					o.SetSurface(this->screen);
					o.SetField(&*warfields.begin());
					o.SetType(5);
					//					o.PrintPos();
					objects.push_back(o);

				}
				lpress = 1;
				break;
			case SDL_BUTTON_RIGHT:
				cout << "R[" << event.motion.y / MAPSIZE << "," << event.motion.x / MAPSIZE << "]="
						<< warfields.begin()->GetMapType(event.motion.x / MAPSIZE, event.motion.y / MAPSIZE) << endl;
				//				cout << "RG[17,36]=" << warfields.begin()->GetMapType(36, 17)
				//						<< endl;
				break;

			}
			break;
		case SDL_MOUSEBUTTONUP:
			lpress = 0;
			break;
		case SDL_KEYDOWN:

			switch (event.key.keysym.sym) {
			case SDLK_0:
				this->objnum = 0;
				break;
			case SDLK_1:
				this->objnum = 1;
				//				cout<<"key 1"<<endl;
				break;
			case SDLK_2:
				this->objnum = 2;
				break;
			case SDLK_3:
				this->objnum = 3;
				break;
			case SDLK_4:
				this->objnum = 4;
				break;
			case SDLK_a:
				shoot = 0;
				break;
			case SDLK_r:

				for (list<Tank>::iterator tank = tanks.begin(); tank != tanks.end(); tank++) {
					warfields.begin()->FillCell(tank->GetRect(), tank->GetType());
				}

				break;
			case SDLK_m:
				if (alpha > SDL_ALPHA_TRANSPARENT) {
					//Decrease alpha
					alpha -= 5;
				}
				break;
			case SDLK_l:
				if (alpha < SDL_ALPHA_OPAQUE) {
					//Increase alpha
					alpha += 5;
				}
				break;
			case SDLK_p:
				//warfields.begin()->PrintMap();

				break;
			case SDLK_c:
				objects.clear();
				warfields.begin()->ClearMap();
				//				cout << "clear map" << endl;
			case SDLK_s:
				//phys.HelloWorld();
				break;
			case SDLK_SPACE:
				//				warfields.begin()->ClearMap();
				//				cout << "clear map" << endl;
				tanks.begin()->Shoot();

				break;
			case SDLK_RCTRL:
			case SDLK_LCTRL:
				tanks.begin()->Shoot();
				break;
			case SDLK_F1:

				e.SetLife(1);
				e.SetMaxLife(1);
				e.SetField(&*(this->warfields.begin()));
				e.SetType(2);
				e.TankBombSound();
				//				effects.push_back(e);

				break;
			case SDLK_F2:
				if (Mix_PlayingMusic() == 0) {
					//Play the music
					if (Mix_PlayMusic(music, -1) == -1) {
						break;
					}
				}
				//If music is being played
				else {
					//If the music is paused
					if (Mix_PausedMusic() == 1) {
						//Resume the music
						Mix_ResumeMusic();
					}
					//If the music is playing
					else {
						//Pause the music
						Mix_PauseMusic();
					}
				}
				break;
			case SDLK_F3:
				//Play the low hit effect
				if (Mix_PlayChannel( -1, chunk, 0 ) == -1) {

				}
				break;
			case SDLK_F4:
				for (int i = 0; i < 10; i++) {
					Tank t;
					t.SetBullets(&bullets);
					t.RandPos();
					t.SetSurface(this->screen);
					t.SetField(&*warfields.begin());

					string name;
					switch (rand() % 3) {
					case 0:
						name = "tank01";
						t.SetType(1000 + i);
						t.SetId(1000 + i);
						t.SetColor(0xff0000);
						break;
					case 1:
						name = "tank02";
						t.SetType(2000 + i);
						t.SetId(2000 + i);
						break;
					case 2:
						t.SetType(3000 + i);
						t.SetId(3000 + i);
						name = "tank03";
						break;
					}
					t.SetName(name);
					this->tanks.push_back(t);
				}
				break;
			case SDLK_F5:
				tanks.begin()->SetLife(100);
				tanks.begin()->SetStep(8);
				break;
			case SDLK_F6:
				for (int i = 0; i < 1000; i++) {
					Tank t;
					t.SetBullets(&bullets);
					t.RandPos();
					t.SetSurface(this->screen);
					t.SetField(&*warfields.begin());

					string name;
					switch (rand() % 3) {
					case 0:
						name = "tank01";
						t.SetType(1000 + i);
						t.SetId(1000 + i);
						t.SetColor(0xff0000);
						break;
					case 1:
						name = "tank02";
						t.SetType(2000 + i);
						t.SetId(2000 + i);
						break;
					case 2:
						t.SetType(3000 + i);
						t.SetId(3000 + i);
						name = "tank03";
						break;
					}
					t.SetName(name);
					this->tanks.push_back(t);
				}
				break;
			case SDLK_F8:
				if (candraw == 0) {
					candraw = 1;
				} else {
					candraw = 0;
				}
				break;

			case SDLK_F10:
				this->map->SaveObject("map.xml");
				break;
			case SDLK_UP:
				//				tanks.begin()->MoveUp();
				direct = 2;
				break;
			case SDLK_DOWN:
				//				tanks.begin()->MoveDown();
				direct = 6;
				break;
			case SDLK_LEFT:
				//				tanks.begin()->MoveLeft();
				direct = 0;
				break;
			case SDLK_RIGHT:
				//				tanks.begin()->MoveRight();
				direct = 4;
				break;
			case SDLK_q:
				//				exit(0);
				//quit = true;
				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:

				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
			case SDLK_DOWN:
			case SDLK_LEFT:
			case SDLK_RIGHT:
				direct = -1;
				break;

			case SDLK_a:
				shoot = -1;
				break;
			case SDLK_w:

				break;

			case SDLK_d:

				break;
			case SDLK_g:

				break;

			case SDLK_t:

				break;
			case SDLK_SPACE:
				//				warfields.begin()->ClearMap();
				//				cout << "clear map" << endl;
				//				tanks.begin()->Shoot();

				break;
			case SDLK_x:

				//PrintKeyInfo( &event.key );
				break;
			case SDL_QUIT:
				printf("Quit requested, quitting.\n");
				quit = false;
				break;
			default:

				break;
			}

		}
	}

}

void Game::Exit() {
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	SDL_FreeSurface(front);
	TTF_CloseFont(font);
	//	Mix_FreeChunk(this->chunk);
	//	Mix_FreeMusic(music);

	resourceManager.FreeAllResource();

	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}
void Game::ShowState() {
	char msg[100];
	sprintf(msg, "FPS:%.2f bullets:%d live:%d tanks:%d live:%d bomb:%d live:%d", fps, (int) bullets.size(),
			(int) bullets.size() - bulletdeadnum, (int) tanks.size(), (int) tanks.size() - tankdeadnum, effects.size(),
			this->bombnum);
	front = TTF_RenderText_Solid(font, msg, textColor);
	apply_surface(10, 10, front, screen, NULL );
}

SDL_Surface *load_image(string filename) {
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	loadedImage = IMG_Load(filename.c_str());
	//loadedImage=SDL_LoadBMP(filename.c_str());
	if (loadedImage != NULL) {
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	return optimizedImage;

}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(source, clip, destination, &offset);
}
void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	if (x < 0 || y < 0 || x > (SCREEN_WIDTH - 1) || y > (SCREEN_HEIGHT - 1))
		return;
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *) p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *) p = pixel;
		break;
	}
}

void Game::Test(SDL_Rect & r) {

}
void put_line(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel) {

	int t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, row, col;
	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	row = x1;
	col = y1;

	if (delta_x > 0) {
		incx = 1; //设置单步方向
	} else if (delta_x == 0) {
		incx = 0;//垂直线
	} else {
		incx = -1;
		delta_x = -delta_x;
	}

	if (delta_y > 0) {
		incy = 1;
	} else if (delta_y == 0) {
		incy = 0;//水平线
	} else {
		incy = -1;
		delta_y = -delta_y;
	}

	if (delta_x > delta_y) {
		distance = delta_x; //选取基本增量坐标轴
	} else {
		distance = delta_y;
	}
	for (t = 0; t <= distance + 1; t++)//画线输出
	{
		put_pixel(surface, row, col, pixel);//画点
		xerr += delta_x;
		yerr += delta_y;

		if (xerr > distance) {
			xerr -= distance;
			row += incx;
		}
		if (yerr > distance) {
			yerr -= distance;
			col += incy;
		}
	}

}

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;

	case 2:
		return *(Uint16 *) p;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return *(Uint32 *) p;

	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}

double calFrequency() {
	static int count;
	static double save;
	static clock_t last, current;
	double timegap;

	++count;
	if (count <= 50)
		return save;
	count = 0;
	last = current;
	current = clock();
	timegap = (current - last) / (double) CLOCKS_PER_SEC;
	save = 50.0 / timegap;
	return save;
}

list<Tank> * GetTanks() {
	return &Game::tanks;
}

}
