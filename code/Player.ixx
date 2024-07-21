#include <SDL.h>

export module Player;

import std;
import util;
import Entity;
import World;
import globalVar;
import textureVar;
import constVar;
import log;
import ItemData;
import nanoTimer;
import globalTime;
import updateBarAct;

export class Player :public Entity //플레이어는 엔티티를 상속받고 시야에 따라 미니맵을 업데이트하는 기능을 가지고 있다.
{
private:
	Player();
public:
	Player(int gridX, int gridY, int gridZ) : Entity(gridX, gridY, gridZ)//생성자입니다.
	{
		static Player* ptr = this;
		prt(L"[디버그] 플레이어 생성 완료 ID : %p\n", this);
		setDrawEquip(true);
		setSprite(spr::charsetHero);
		loadDataFromDex(1);
		(World::ins())->getTile(0, 0, 0).EntityPtr = this;
		setTalentFocus(talentFlag::fighting, 1);
		setTalentFocus(talentFlag::dodging, 1);
		setTalentFocus(talentFlag::stealth, 1);


		setSkin(humanCustom::skin::yellow);
		setEyes(humanCustom::eyes::blue);
		setHair(humanCustom::hair::middlePart);

		int i = 0;

		getEquipPtr()->addItemFromDex(2);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		getEquipPtr()->addItemFromDex(290);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		getEquipPtr()->addItemFromDex(105);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		getEquipPtr()->addItemFromDex(106);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		getEquipPtr()->addItemFromDex(107);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		getEquipPtr()->addItemFromDex(374);
		getEquipPtr()->itemInfo[i++].equipState = equip::normal;

		addSkill(27);
		quickSlot[0] = { quickSlotFlag::SKILL, 27 };

		addSkill(19);
		quickSlot[1] = { quickSlotFlag::SKILL, 19 };

		addSkill(30);
		quickSlot[2] = { quickSlotFlag::SKILL, 30 };

		addSkill(14);
		quickSlot[3] = { quickSlotFlag::SKILL, 14 };

		addSkill(10);
		quickSlot[4] = { quickSlotFlag::SKILL, 10 };

		addSkill(1);
		quickSlot[5] = { quickSlotFlag::SKILL, 1 };
	}
	~Player()
	{
		prt(L"Player : 소멸자가 호출되었습니다..\n");
	}
	static Player* ins()//싱글톤 함수
	{
		static Player* ptr = new Player(0, 0, 0);
		ptr->setIsPlayer(true);
		return ptr;
	}

	virtual void startAtk(int inputGridX, int inputGridY, int inputGridZ, int inputTarget, aniFlag inputAniType) override
	{
		Entity::startAtk(inputGridX, inputGridY, inputGridZ, inputTarget, inputAniType);
		addAniUSetPlayer(this, inputAniType);
	}

	void startAtk(int inputGridX, int inputGridY, int inputGridZ, int inputTarget) { startAtk(inputGridX, inputGridY, inputGridZ, inputTarget, aniFlag::atk); }

	void startAtk(int inputGridX, int inputGridY, int inputGridZ) { startAtk(inputGridX, inputGridY, inputGridZ, -1); }

	void startMove(int inputDir)
	{
		if (Player::ins()->getAniType() == aniFlag::null)
		{
			//errorBox(Player::ins()->getAniType() == aniFlag::null, "Player's startMove activated while player's aniFlag is not null.");
			errorBox(((Player::ins())->getX() - 8) % 16 != 0, "This instance moved from non-integer coordinates.");

			int dx, dy;
			dir2Coord(inputDir, dx, dy);
			Player* player = Player::ins();
			player->updateWalkable(player->getGridX() + dx, player->getGridY() + dy);
			//걸을 수 있는 타일이면
			if (World::ins()->getTile(player->getGridX() + dx, player->getGridY() + dy, player->getGridZ()).walkable)
			{
				player->setDirection(inputDir);
				player->move(inputDir, false);
				turnCycle = turn::playerAnime;
			}
			else
			{
				player->setDirection(inputDir);
				if (World::ins()->getTile(player->getGridX() + dx, player->getGridY() + dy, player->getGridZ()).EntityPtr != nullptr)
				{
					player->startAtk(player->getGridX() + dx, player->getGridY() + dy, player->getGridZ());
					turnWait(1.0);
					Player::ins()->deactAStarDst();
				}
			}
		}
	}

	void updateMinimap()
	{
		auto timeStampStart = getNanoTimer();
		SDL_SetRenderTarget(renderer, texture::minimap);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		for (int dx = -(MINIMAP_DIAMETER / 2); dx <= (MINIMAP_DIAMETER / 2); dx++)
		{
			for (int dy = -(MINIMAP_DIAMETER / 2); dy <= (MINIMAP_DIAMETER / 2); dy++)
			{
				if (isCircle(MINIMAP_DIAMETER / 2, dx, dy))
				{
					SDL_Color ptCol;
					const TileData* tgtTile = &World::ins()->getTile(getGridX() + dx, getGridY() + dy, getGridZ());
					if (tgtTile->fov == fovFlag::white || tgtTile->fov == fovFlag::gray)
					{
						//floor
						switch (tgtTile->floor)
						{
						case 0:
							break;
						default:
							ptCol = { 112,112, 112 };
							break;
						}

						//wall
						switch (tgtTile->wall)
						{
						case 0:
							break;
						default:
							ptCol = { 29,29, 29 };
							break;
						}
						
						//prop
						if (tgtTile->PropPtr != nullptr) ptCol = lowCol::yellow;
						//vehicle
						if (tgtTile->VehiclePtr != nullptr) ptCol = lowCol::orange;

						drawPoint(dx + (MINIMAP_DIAMETER / 2), dy + (MINIMAP_DIAMETER / 2),ptCol);

						if (tgtTile->fov == fovFlag::gray) drawPoint(dx + (MINIMAP_DIAMETER / 2), dy + (MINIMAP_DIAMETER / 2), col::black, 100);
					}
					else drawPoint(dx + (MINIMAP_DIAMETER / 2), dy + (MINIMAP_DIAMETER / 2), col::black);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawPoint(renderer, (MINIMAP_DIAMETER / 2), (MINIMAP_DIAMETER / 2));

		SDL_SetRenderTarget(renderer, nullptr);
	}


	void updateVision(int range, int cx, int cy)
	{
		__int64 timeStampStart = getNanoTimer();
		//prt(L"[updateVision] %d,%d에서 시야업데이트가 진행되었다.\n",cx,cy);

		int correctionRange = range;
		if (getHour() >= 6 && getHour() < 18) correctionRange = range;
		else correctionRange = range / 2;

		//줌스케일이 최대일 때 45칸 정도가 최대로 들어옴
		for (int i = cx - GRAY_VISION_HALF_W; i <= cx + GRAY_VISION_HALF_W; i++)
		{
			for (int j = cy - GRAY_VISION_HALF_H; j <= cy + GRAY_VISION_HALF_H; j++)
			{
				TileData* tgtTile = &World::ins()->getTile(i, j, getGridZ());
				if (tgtTile->fov == fovFlag::white) tgtTile->fov = fovFlag::gray;
			}
		}


		std::vector<Point2> tasksVec;
		tasksVec.reserve((2 * DARK_VISION_HALF_W + 1) * (2 * DARK_VISION_HALF_H + 1));
		for (int tgtX = cx - DARK_VISION_HALF_W; tgtX <= cx + DARK_VISION_HALF_W; tgtX++)
		{
			for (int tgtY = cy - DARK_VISION_HALF_H; tgtY <= cy + DARK_VISION_HALF_H; tgtY++)
			{
				tasksVec.push_back({ tgtX,tgtY });
			}
		}

		auto rayCastingWorker = [=](int cx, int cy, const std::vector<Point2>& points, int correctionRange)
			{
				for (const auto& point : points)
				{
					if (isCircle(correctionRange, point.x - cx, point.y - cy)) rayCasting(cx, cy, point.x, point.y);
					else rayCastingDark(cx, cy, point.x, point.y);
				}
			};

		int numThreads = threadPoolPtr->getAvailableThreads();
		int chunkSize = tasksVec.size() / numThreads;
		for (int i = 0; i < numThreads; i++) {
			std::vector<Point2>::iterator startPoint = tasksVec.begin() + i * chunkSize;

			std::vector<Point2>::iterator endPoint;
			if (i == numThreads - 1) endPoint = tasksVec.end(); //만약 마지막 스레드일 경우 벡터의 끝을 강제로 설정
			else endPoint = startPoint + chunkSize;
			std::vector<Point2> chunk(startPoint, endPoint);

			threadPoolPtr->addTask([=]() {
				rayCastingWorker(cx, cy, chunk, correctionRange);
				});
		}

		threadPoolPtr->waitForThreads();
	}


	void updateVision(int range) {
		updateVision(range, getGridX(), getGridY());
	}


	void updateNearbyChunk(int range)
	{
		int chunkX, chunkY;
		World::ins()->changeToChunkCoord(getGridX(), getGridY(), chunkX, chunkY);
		for (int y = chunkY - range; y <= chunkY + range; y++)
		{
			for (int x = chunkX - range; x <= chunkX + range; x++)
			{
				if (World::ins()->existChunk(x, y, getGridZ()) == false)
				{
					World::ins()->createChunk(x, y, getGridZ());
				}
			}
		}

		for (int y = chunkY - range; y <= chunkY + range; y++)
		{
			for (int x = chunkX - range; x <= chunkX + range; x++)
			{
				if (World::ins()->existChunk(x, y, getGridZ() + 1) == false)
				{
					World::ins()->createChunk(x, y, getGridZ() + 1);
				}
			}
		}

		for (int y = chunkY - range; y <= chunkY + range; y++)
		{
			for (int x = chunkX - range; x <= chunkX + range; x++)
			{
				if (World::ins()->existChunk(x, y, getGridZ() - 1) == false)
				{
					World::ins()->createChunk(x, y, getGridZ() - 1);
				}
			}
		}

		World::ins()->deactivate();

		for (int y = chunkY - 1; y <= chunkY + 1; y++)
		{
			for (int x = chunkX - 1; x <= chunkX + 1; x++)
			{
				World::ins()->activate(x, y, ins()->getGridZ());
			}
		}
	}

	void setGrid(int inputGridX, int inputGridY, int inputGridZ) override
	{
		Coord::setGrid(inputGridX, inputGridY, inputGridZ);

		std::array<int, 2> sectorXY = World::ins()->changeToSectorCoord(getGridX(), getGridY());
		for (int dir = -1; dir <= 7; dir++)
		{
			int dx, dy;
			dir2Coord(dir, dx, dy);
			if (World::ins()->isEmptySector(sectorXY[0] + dx, sectorXY[1] + dy, getGridZ()) == true) World::ins()->createSector(sectorXY[0] + dx, sectorXY[1] + dy, getGridZ());
		}
		updateNearbyChunk(CHUNK_LOADING_RANGE);
		updateNearbyBarAct(inputGridX, inputGridY, inputGridZ);
	}

	void endMove() override //aStar로 인해 이동이 끝났을 경우
	{
		updateVision(getEyeSight());
		updateMinimap();
	}

	void death()
	{
		updateLog(L"#FFFFFF죽어버렸다~~!!!!.");
	}

	int checkItemSur(int index)//주변에 있는 타일을 포함해 아이템을 가지고 있는지 조사
	{
		int itemNumber = 0;
		//주변 9타일의 아이템스택 검사
		for (int i = 0; i < 9; i++)
		{
			ItemStack* ptr = World::ins()->getItemPos(getGridX(), getGridY(), getGridZ());
		}
		//자기 자신의 장비 검사 
		{
			itemNumber++;
		}
		return itemNumber;
	}
	void eraseItemSur(int index, int number) //주변객체를 중심으로 총 9칸
	{
		int residue = number;
		//주변 9타일의 아이템스택 검사
		for (int i = 0; i < 9; i++)
		{
			residue--;
			if (residue == 0) { return; }
		}
		//자기 자신의 장비 검사 
		{
			residue--;
			if (residue == 0) { return; }
		}
	}
	int checkToolQualitySur(int index) //없으면 0 반환, 있으면 공구레벨 반환
	{
		int itemNumber = 0;
		//주변 9타일의 아이템스택 검사
		for (int i = 0; i < 9; i++)
		{
			itemNumber++;
		}
		//자기 자신의 장비 검사 
		{
			itemNumber++;
		}
		return itemNumber;
	}
};