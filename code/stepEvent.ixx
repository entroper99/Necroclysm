﻿#include <SDL3/SDL.h>

export module stepEvent;

import globalVar;
import wrapVar;
import constVar;
import util;
import Damage;
import Corpse;
import Particle;
import Footprint;
import Wave;
import GUI;
import mouseInput;
import log;
import Player;
import World;
import textureVar;
import globalTime;
import Wake;


//GUI의 스텝이벤트를 실행시키는 함수

export __int64 stepEvent()
{
    __int64 timeStampStart = getNanoTimer();

	////게임패드 감지 및 입력방식 변경
	//int numJoy = 0;
	//SDL_JoystickID* list = SDL_GetJoysticks(&numJoy);
	//if (numJoy > 0 && list)
	//{
	//	if (controller == nullptr)
	//	{
	//		for (int i = 0; i < numJoy; ++i)
	//		{
	//			SDL_JoystickID jid = list[i];
	//			if (SDL_IsGamepad(jid))
	//			{
	//				controller = SDL_OpenGamepad(jid);
	//				if (controller)
	//				{
	//					std::wstring str = L"다음 게임패드가 감지되었다. : ";
	//					str += stringToWstring(SDL_GetGamepadName(controller));
	//					updateLog(str);
	//					option::inputMethod = input::gamepad;
	//					break;
	//				}
	//				else
	//				{
	//					errorBox(L"게임패드를 열 수가 없다.");
	//				}
	//			}
	//		}
	//	}
	//}
	//else controller = nullptr;






    if (dxClickStack > TOLERANCE_HOLD_DEL_XY || dyClickStack > TOLERANCE_HOLD_DEL_XY) deactHold = true;

    if (getMilliTimer() - clickStartTime > 1000)
    {
        if (deactHold == false)
        {
            clickHold();
        }
    }
	//데미지 객체 스탭 이벤트 실행
	for (int i = 0; i < Damage::list.size(); i++){Damage::list[i]->step();}

    //시체 객체 스텝 이벤트 실행
	for (int i = 0; i < Corpse::list.size(); i++){Corpse::list[i]->step();}

	//파티클 객체 스탭 이벤트 실행
	for (int i = 0; i < Particle::list.size(); i++) { Particle::list[i]->step(); }

	//파티클 객체 스탭 이벤트 실행
	for (int i = 0; i < Footprint::list.size(); i++) { Footprint::list[i]->step(); }
	for (int i = 0; i < Wave::list.size(); i++) { Wave::list[i]->step(); }
	for (int i = 0; i < Wake::list.size(); i++) { Wake::list[i]->step(); }


    //GUI 객체 스텝 이벤트 실행
    for (int i = 0; i < GUI::getActiveGUIList().size(); i++){GUI::getActiveGUIList()[i]->step();}

	int cx, cy;
	int pz = PlayerZ();
	World::ins()->changeToChunkCoord(PlayerX(), PlayerY(), cx, cy);
	if (World::ins()->getChunkWeather(cx, cy, pz) == weatherFlag::snow)
	{
		int randX = randomRange(0, cameraW);
		int randY = randomRange(0, cameraH);
		snowflakes.push_back(std::make_unique<Snowflake>(randX,randY,randomRange(2,3)));
	}
	else if (World::ins()->getChunkWeather(cx, cy, pz) == weatherFlag::rain)
	{
		{
			int randX = randomRange(0, cameraW + 100);
			int randY = randomRange(0 - 100, cameraH);
			raindrops.push_back(std::make_unique<Raindrop>(randX, randY));
		}

		{
			int randX = randomRange(0, cameraW + 100);
			int randY = randomRange(0 - 100, cameraH);
			raindrops.push_back(std::make_unique<Raindrop>(randX, randY));
		}

		{
			int randX = randomRange(0, cameraW + 100);
			int randY = randomRange(0 - 100, cameraH);
			raindrops.push_back(std::make_unique<Raindrop>(randX, randY));
		}
	}

	for (int i = snowflakes.size() - 1; i >= 0; --i) 
	{
		if (snowflakes[i]->y < snowflakes[i]->dstY) 
		{
			snowflakes[i]->y += 2;
		}
		else
		{
			if (snowflakes[i]->alpha > 0)
			{
				if (snowflakes[i]->alpha >= 10) snowflakes[i]->alpha -= 10;
				else snowflakes[i]->alpha = 0;
			}
			else
			{
				snowflakes.erase(snowflakes.begin() + i);

			}
		}
	}

	for (int i = raindrops.size() - 1; i >= 0; --i)
	{
		if (raindrops[i]->y < raindrops[i]->dstY)
		{
			int dx = cos((raindrops[i].get())->angle) * (raindrops[i].get())->velocity;
			int dy = sin((raindrops[i].get())->angle) * (raindrops[i].get())->velocity;

			raindrops[i]->x -= dx;
			raindrops[i]->y += dy;
		}
		else
		{
			if (raindrops[i]->length > 0)
			{
				raindrops[i]->length -= 2.0;
				raindrops[i]->alpha -= 10;

				if (raindrops[i]->length < 3.0)
				{
					raindrops[i]->alpha = 255;
				}
			}
			else
			{
				raindrops.erase(raindrops.begin() + i);
			}

		}
	}

	//for (int i = raindrops.size() - 1; i >= 0; --i)
	//{
	//	if (raindrops[i]->y < 720)
	//	{
	//		int dx = cos((raindrops[i].get())->angle) * (raindrops[i].get())->velocity;
	//		int dy = sin((raindrops[i].get())->angle) * (raindrops[i].get())->velocity;

	//		raindrops[i]->x -= dx;
	//		raindrops[i]->y += dy;
	//	}
	//	else
	//	{
	//		raindrops.erase(raindrops.begin() + i);
	//	}
	//}


    return (getNanoTimer() - timeStampStart);
}