#include <SDL.h>

export module ContextMenu;

import std;
import util;
import GUI;
import textureVar;
import drawText;
import drawSprite;
import globalVar;
import wrapVar;
import checkCursor;
import drawWindow;
import actFuncSet;
import Player;
import Loot;
import World;
import Vehicle;
import Bionic;
import log;

export class ContextMenu : public GUI
{
private:
	inline static ContextMenu* ptr = nullptr;
	SDL_Rect contextMenuBase;
	SDL_Rect subContextMenuBase;
	int contextMenuCursor = -1;
	int contextMenuScroll = 0;
	std::vector<act> actOptions;
	Point2 targetGrid; //컨텍스트메뉴가 액트를 얻어낼 그리드 좌표
	std::array<SDL_Rect,30> optionRect;

public:
	ContextMenu(int inputMouseX, int inputMouseY, int inputGridX, int inputGridY, std::vector<act> inputOptions) : GUI(false)
	{
		actOptions = inputOptions;
		targetGrid = { inputGridX, inputGridY };
		//1개 이상의 메시지 객체 생성 시의 예외 처리
		errorBox(ptr != nullptr, "More than one message instance was generated.");
		ptr = this;

		//메세지 박스 렌더링
		changeXY(inputMouseX, inputMouseY, false);
		tabType = tabFlag::closeWin;

		deactInput();
		deactDraw();
		addAniUSetPlayer(this, aniFlag::winUnfoldOpen);
	}
	~ContextMenu()
	{
		tabType = tabFlag::autoAtk;
		ptr = nullptr;
	}
	static ContextMenu* ins() { return ptr; }
	void changeXY(int inputX, int inputY, bool center)
	{
		//94
		contextMenuBase = { 0, 0, 110, 6 + 22 * (int)actOptions.size() };
		subContextMenuBase = { 0,0,0,0 };

		

		if (center == false)
		{
			contextMenuBase.x += inputX;
			contextMenuBase.y += inputY;
		}
		else
		{
			contextMenuBase.x += inputX - contextMenuBase.w / 2;
			contextMenuBase.y += inputY - contextMenuBase.h / 2;
		}


		if (center == false)
		{
			x = inputX;
			y = inputY;
		}
		else
		{
			x = inputX - contextMenuBase.w / 2;
			y = inputY - contextMenuBase.h / 2;
		}

		for (int i = 0; i < 30; i++) optionRect[i] = { contextMenuBase.x + 4, contextMenuBase.y + 4 + 22 * i, contextMenuBase.w - 8, 20 };

	}
	void drawGUI()
	{
		if (getStateDraw() == false) { return; }

		if (getFoldRatio() == 1.0)
		{

			drawFillRect(contextMenuBase, col::black);
			drawRect(contextMenuBase, col::gray);

			//94 110
			for (int i = 0; i < actOptions.size(); i++)
			{
				//64,65
				std::wstring optionText;
				int iconIndex = 0;
				if (actOptions[i] == act::closeDoor)
				{
					optionText = sysStr[176];//닫기
					iconIndex = 64;
				}
				else if (actOptions[i] == act::inspect)
				{
					optionText = sysStr[177];//조사
					iconIndex = 65;
				}
				else if (actOptions[i] == act::unbox)
				{
					optionText = sysStr[178];//열기
					iconIndex = 66;
				}
				else if (actOptions[i] == act::pull)
				{
					optionText = sysStr[179];//당기기
					iconIndex = 67;
				}
				else if (actOptions[i] == act::climb)
				{
					optionText = sysStr[188];//등반
					iconIndex = 69;
				}
				else if (actOptions[i] == act::swim)
				{
					optionText = sysStr[186];//수영
					iconIndex = 70;
				}
				else if (actOptions[i] == act::ride)
				{
					optionText = sysStr[187];//탑승
					iconIndex = 71;
				}
				else optionText = L"???";

				if (checkCursor(&optionRect[i]))
				{
					if (click) drawFillRect(optionRect[i], lowCol::deepBlue);
					else  drawFillRect(optionRect[i], lowCol::blue);
				}
				else drawFillRect(optionRect[i], lowCol::black);
				setFontSize(16);
				drawTextCenter(col2Str(col::white) + optionText, optionRect[i].x + optionRect[i].w / 2 + 16, optionRect[i].y + optionRect[i].h / 2);
				drawSpriteCenter(spr::icon16, iconIndex, optionRect[i].x + 10, optionRect[i].y + 10);
			}
			
			// 좌측상단
			drawLine(contextMenuBase.x, contextMenuBase.y, contextMenuBase.x + 12, contextMenuBase.y, col::lightGray);
			drawLine(contextMenuBase.x, contextMenuBase.y + 1, contextMenuBase.x + 12, contextMenuBase.y + 1, col::lightGray);
			drawLine(contextMenuBase.x, contextMenuBase.y, contextMenuBase.x, contextMenuBase.y + 12, col::lightGray);
			drawLine(contextMenuBase.x + 1, contextMenuBase.y, contextMenuBase.x + 1, contextMenuBase.y + 12, col::lightGray);

			// 우측상단
			drawLine(contextMenuBase.x + contextMenuBase.w - 13, contextMenuBase.y, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 13, contextMenuBase.y + 1, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + 1, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + 12, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 2, contextMenuBase.y, contextMenuBase.x + contextMenuBase.w - 2, contextMenuBase.y + 12, col::lightGray);

			// 좌측하단
			drawLine(contextMenuBase.x, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x + 12, contextMenuBase.y + contextMenuBase.h - 1, col::lightGray);
			drawLine(contextMenuBase.x, contextMenuBase.y + contextMenuBase.h - 2, contextMenuBase.x + 12, contextMenuBase.y + contextMenuBase.h - 2, col::lightGray);
			drawLine(contextMenuBase.x, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x, contextMenuBase.y + contextMenuBase.h - 13, col::lightGray);
			drawLine(contextMenuBase.x + 1, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x + 1, contextMenuBase.y + contextMenuBase.h - 13, col::lightGray);

			// 우측하단
			drawLine(contextMenuBase.x + contextMenuBase.w - 13, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + contextMenuBase.h - 1, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 13, contextMenuBase.y + contextMenuBase.h - 2, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + contextMenuBase.h - 2, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x + contextMenuBase.w - 1, contextMenuBase.y + contextMenuBase.h - 13, col::lightGray);
			drawLine(contextMenuBase.x + contextMenuBase.w - 2, contextMenuBase.y + contextMenuBase.h - 1, contextMenuBase.x + contextMenuBase.w - 2, contextMenuBase.y + contextMenuBase.h - 13, col::lightGray);


			drawFillRect(subContextMenuBase, col::black);

		}
		else
		{
			SDL_Rect vRect = contextMenuBase;
			int type = 1;
			switch (type)
			{
			case 0:
				vRect.w = vRect.w * getFoldRatio();
				vRect.h = vRect.h * getFoldRatio();
				break;
			case 1:
				vRect.x = vRect.x + vRect.w * (1 - getFoldRatio()) / 2;
				vRect.w = vRect.w * getFoldRatio();
				break;
			}
			drawWindow(&vRect);
		}
	}
	void clickUpGUI()
	{
		if (getStateInput() == false) { return; }

		if (checkCursor(&tab))
		{
			close(aniFlag::winUnfoldClose);
		}
		else if(checkCursor(&contextMenuBase))
		{
			for (int i = 0; i < actOptions.size(); i++)
			{
				if (checkCursor(&optionRect[i]))
				{
					if (actOptions[i] == act::closeDoor)
					{
						actFunc::closeDoor(targetGrid.x, targetGrid.y, PlayerZ());
						break;
					}
					else if (actOptions[i] == act::unbox)
					{
						if (World::ins()->getTile(targetGrid.x, targetGrid.y, PlayerZ()).VehiclePtr != nullptr)
						{
							Vehicle* vPtr = (Vehicle*)World::ins()->getTile(targetGrid.x, targetGrid.y, PlayerZ()).VehiclePtr;
							for (int i = 0; i < vPtr->partInfo[{targetGrid.x, targetGrid.y}]->itemInfo.size(); i++)
							{
								if (vPtr->partInfo[{targetGrid.x, targetGrid.y}]->itemInfo[i].checkFlag(itemFlag::POCKET))
								{
									new Loot((ItemPocket*)(vPtr->partInfo[{targetGrid.x, targetGrid.y}]->itemInfo[i].pocketPtr), &(vPtr->partInfo[{targetGrid.x, targetGrid.y}]->itemInfo[i]));
									break;
								}
							}
						}
						break;
					}
					else if (actOptions[i] == act::pull)
					{
						if (Player::ins()->pulledCart == nullptr)
						{
							Player::ins()->pulledCart = (Vehicle*)World::ins()->getTile(targetGrid.x, targetGrid.y, PlayerZ()).VehiclePtr;
						}
						else
						{
							Player::ins()->pulledCart = nullptr;
						}
						break;
					}
					else if (actOptions[i] == act::ride)
					{
						Player::ins()->ridingEntity = TileEntity(targetGrid.x, targetGrid.y, PlayerZ());
						World::ins()->getTile(targetGrid.x, targetGrid.y, PlayerZ()).EntityPtr = nullptr;
						Player::ins()->ridingType = ridingFlag::horse;
						break;
					}
				}

			}

			close(aniFlag::null);
		}
		else close(aniFlag::winUnfoldClose);
	}
	void clickMotionGUI(int dx, int dy) { }
	void clickDownGUI() { }
	void clickRightGUI() { }
	void clickHoldGUI() { }
	void gamepadBtnDown() { }
	void gamepadBtnMotion() { }
	void gamepadBtnUp() { }
	void step() 
	{
		contextMenuBase.h = 6 + 22 * (int)actOptions.size();
		//bool openSub = false;
		//for (int i = 0; i < actOptions.size(); i++)
		//{
		//	if (actOptions[i] == act::inspect)
		//	{
		//		SDL_Rect optionRect = { contextMenuBase.x + 4, contextMenuBase.y + 4 + 22 * i, 90, 20 };
		//		if (checkCursor(&optionRect) || checkCursor(&subContextMenuBase))
		//		{
		//			subContextMenuBase = { contextMenuBase.x + contextMenuBase.w, contextMenuBase.y + 4 + 22 * i - 1, contextMenuBase.w,100 };
		//			openSub = true;
		//		}
		//	}
		//}

		//if(openSub==false) subContextMenuBase = { -1, -1, -1,-1 };
	}
};