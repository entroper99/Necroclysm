﻿import HUD;

#include <SDL3/SDL.h>

import std;
import util;
import globalVar;
import wrapVar;
import constVar;
import Player;
import World;
import Prop;
import Aim;


void HUD::gamepadBtnDown()
{
	switch (event.gbutton.button)
	{
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
		break;
	case SDL_GAMEPAD_BUTTON_NORTH:
		if (y == 0)
		{
			executePopUp();
			barActCursor = 0;
		}
		else
		{
			executePopDown();
			barActCursor = -1;
		}
		break;
	case SDL_GAMEPAD_BUTTON_WEST:
		quickSlotToggle();
		if (isQuickSlotPop) quickSlotCursor = 0;
		else quickSlotCursor = -1;
		break;
	case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
		zoomScale += 1;
		if (zoomScale > 5.0) zoomScale = 1.0;
		break;
	}


}
void HUD::gamepadBtnMotion() { }
void HUD::gamepadBtnUp()
{
	switch (event.gbutton.button)
	{
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
		if (isQuickSlotPop && quickSlotCursor > 0) quickSlotCursor--;
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		if (isQuickSlotPop && quickSlotCursor < 7) quickSlotCursor++;
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
		break;
	case SDL_GAMEPAD_BUTTON_SOUTH:
		if (barActCursor != -1) clickLetterboxBtn(barAct[barActCursor]);
		else
		{
			__int16 leftX = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX);
			__int16 leftY = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY);

			int tgtX = PlayerX();
			int tgtY = PlayerY();

			if (leftX > TOLERANCE_LSTICK) tgtX += 1;
			if (leftX < -TOLERANCE_LSTICK) tgtX -= 1;
			if (leftY > TOLERANCE_LSTICK) tgtY += 1;
			if (leftY < -TOLERANCE_LSTICK) tgtY -= 1;

			tileTouch(tgtX, tgtY);
		}
		break;
	case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
		break;
	case SDL_GAMEPAD_BUTTON_START:
		__int16 leftX = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX);
		__int16 leftY = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY);

		int tgtX = PlayerX();
		int tgtY = PlayerY();

		if (leftX > TOLERANCE_LSTICK) tgtX += 1;
		if (leftX < -TOLERANCE_LSTICK) tgtX -= 1;
		if (leftY > TOLERANCE_LSTICK) tgtY += 1;
		if (leftY < -TOLERANCE_LSTICK) tgtY -= 1;

		openContextMenu({ tgtX,tgtY });

	}
}
void HUD::gamepadStep()
{
	if (option::inputMethod == input::gamepad)
	{
		if (dpadDelay <= 0)
		{
			dpadDelay = 6;
			int dir = -1;
			SDL_PollEvent(&event);
			bool dpadUpPressed = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_UP);
			bool dpadDownPressed = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
			bool dpadLeftPressed = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
			bool dpadRightPressed = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

			if (dpadUpPressed && dpadLeftPressed) dir = 3;
			else if (dpadUpPressed && dpadRightPressed) dir = 1;
			else if (dpadDownPressed && dpadLeftPressed) dir = 5;
			else if (dpadDownPressed && dpadRightPressed) dir = 7;
			else if (dpadUpPressed) dir = 2;
			else if (dpadDownPressed) dir = 6;
			else if (dpadLeftPressed) dir = 4;
			else if (dpadRightPressed) dir = 0;

			if (barActCursor == -1 && quickSlotCursor == -1)
			{
				if (dir != -1)
				{
					int dx, dy;
					dir2Coord(dir, dx, dy);
					if (isWalkable({ PlayerX() + dx, PlayerY() + dy, PlayerZ() }) == true)//1칸 이내
					{
						cameraFix = true;
						PlayerPtr->startMove(dir);
					}
					else
					{
						int dx, dy;
						dir2Coord(dir, dx, dy);
						Prop* tgtProp = TileProp(PlayerX() + dx, PlayerY() + dy, PlayerZ());
						if (tgtProp != nullptr)
						{
							int tgtItemCode = tgtProp->leadItem.itemCode;
							if (tgtProp->leadItem.checkFlag(itemFlag::DOOR_CLOSE))
							{
								if (tgtProp->leadItem.checkFlag(itemFlag::PROP_WALKABLE) == false)
								{
									tgtProp->leadItem.eraseFlag(itemFlag::DOOR_CLOSE);
									tgtProp->leadItem.addFlag(itemFlag::DOOR_OPEN);

									tgtProp->leadItem.addFlag(itemFlag::PROP_WALKABLE);
									tgtProp->leadItem.eraseFlag(itemFlag::PROP_BLOCKER);
									tgtProp->leadItem.extraSprIndexSingle++;

									if (tgtProp->leadItem.checkFlag(itemFlag::PROP_GAS_OBSTACLE_ON))
									{
										tgtProp->leadItem.eraseFlag(itemFlag::PROP_GAS_OBSTACLE_ON);
										tgtProp->leadItem.addFlag(itemFlag::PROP_GAS_OBSTACLE_OFF);
									}

									PlayerPtr->updateVision(PlayerPtr->entityInfo.eyeSight);
								}
							}
						}
					}
				}
			}
		}
		else dpadDelay--;

		if (barActCursorMoveDelay <= 0 && barActCursor != -1)
		{
			barActCursorMoveDelay = 5;
			if (dpadUpPressed)
			{
				if (barActCursor >= 7) barActCursor -= 7;
			}
			else if (dpadDownPressed)
			{
				if (barAct.size() - 1 >= barActCursor + 7) barActCursor += 7;
			}
			else if (dpadLeftPressed)
			{
				if (barActCursor % 7 == 0)
				{
					barActCursor += 6;
					if (barActCursor >= barAct.size() - 1) barActCursor = barAct.size() - 1;
				}
				else barActCursor--;
			}
			else if (dpadRightPressed)
			{
				if (barActCursor % 7 == 6) barActCursor -= 6;
				else
				{
					barActCursor++;
					if (barActCursor >= barAct.size() - 1) barActCursor = barAct.size() - 1;
				}
			}
			else barActCursorMoveDelay = 0;
		}
		else barActCursorMoveDelay--;


		if (GUI::getLastGUI() == this)
		{
			if (delayR2 <= 0 && SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 1000)
			{
				prt(L"[HUD:STEP] 탭이 실행되었다.\n");
				executeTab();
				delayR2 = 20;
			}
			else
			{
				delayR2--;
			}
		}

		__int16 rightX = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTX); // -32768 ~ 32767
		__int16 rightY = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTY); // -32768 ~ 32767
		const int maxDist = 160;
		int prevCameraX = cameraX, prevCameraY = cameraY;
		if (rightY < -8000)
		{
			cameraFix = false;
			cameraY -= 2;
		}
		else if (rightY > 8000)
		{
			cameraFix = false;
			cameraY += 2;
		}

		if (rightX < -8000)
		{
			cameraFix = false;
			cameraX -= 2;
		}
		else if (rightX > 8000)
		{
			cameraFix = false;
			cameraX += 2;
		}

		if (std::abs(PlayerPtr->getX() - cameraX) > maxDist) cameraX = prevCameraX;
		if (std::abs(PlayerPtr->getY() - cameraY) > maxDist) cameraY = prevCameraY;




		__int16 leftX = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX);
		__int16 leftY = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY);
		int tgtX = PlayerX();
		int tgtY = PlayerY();
		if (leftX > TOLERANCE_LSTICK) tgtX += 1;
		if (leftX < -TOLERANCE_LSTICK) tgtX -= 1;
		if (leftY > TOLERANCE_LSTICK) tgtY += 1;
		if (leftY < -TOLERANCE_LSTICK) tgtY -= 1;

		if (!(tgtX == PlayerX() && tgtY == PlayerY()))
		{
			gamepadWhiteMarker.x = tgtX;
			gamepadWhiteMarker.y = tgtY;
			gamepadWhiteMarker.z = PlayerZ();
		}
		else gamepadWhiteMarker.z = std::numeric_limits<int>::max();
	}
}