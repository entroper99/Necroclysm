import util;
import Entity;
import globalVar;
import wrapVar;
import constVar;
import textureVar;
import turnWait;
import World;
import Sticker;
import Flame;
import Player;
import Prop;

import Bullet;
import Aim;
import TileData;
import ItemStack;
import Particle;

bool Entity::runAnimation(bool shutdown)
{
	//if (isPlayer) prt(L"Player의 runAnimation이 실행되었다.\n");
	//else prt(L"Entity %p의 runAnimation이 실행되었다.\n",this);

	if (getAniType() == aniFlag::move)//만약 플레이어 인스턴스의 좌표와 목적좌표가 다를 경우
	{
		//8프레임-4(스피드2)
		//6프레임-3(스피드3)
		//4프레임-2(스피드4)

		// 1 / 60초마다 runAnimation이 실행됨

		addTimer();

		if (getTimer() == 1)
		{
			footChanged = false;
			setFakeX(0);
			setFakeY(0);
		}

		if (getX() + getIntegerFakeX() > getDstX()) addFakeX(-entityInfo.gridMoveSpd);
		else if (getX() + getIntegerFakeX() < getDstX()) addFakeX(+entityInfo.gridMoveSpd);
		if (getY() + getIntegerFakeY() > getDstY()) addFakeY(-entityInfo.gridMoveSpd);
		else if (getY() + getIntegerFakeY() < getDstY()) addFakeY(+entityInfo.gridMoveSpd);

		if (entityInfo.isPlayer)
		{
			cameraFix = false;
			cameraX = getX() + getIntegerFakeX();
			cameraY = getY() + getIntegerFakeY();
		}

		if (std::abs(getIntegerFakeX()) >= 8.0 || std::abs(getIntegerFakeY()) >= 8.0)
		{
			if (footChanged == false)
			{
				if (getLeftFoot() == true)
				{
					setSpriteIndex(1);
					setLeftFoot(false);
				}
				else
				{
					setSpriteIndex(2);
					setLeftFoot(true);
				}
				footChanged = true;
			}
		}

		if (std::abs(getIntegerFakeX()) >= 16.0 || std::abs(getIntegerFakeY()) >= 16.0)
		{
			setSpriteIndex(0);
			resetTimer();
			setAniType(aniFlag::null);
			setFakeX(0);
			setFakeY(0);
			setGrid(getDstGridX(), getDstGridY(), getGridZ());
			turnWait(1.0);

			for (int i = 0; i < lightList.size(); i++)
			{
				auto lPtr = lightList[i].get();
				lPtr->releaseLight();
				lPtr->setGrid(getGridX(), getGridY(), getGridZ());
				lPtr->updateLight(lPtr->lightRange);
			}

			endMove();
			if (entityInfo.isPlayer) cameraFix = true;
			return true;
		}
	}
	else if (getAniType() == aniFlag::atk)
	{
		addTimer();

		char dx;
		char dy;

		static bool rightAtk = true;

		switch (entityInfo.direction)
		{
		case 0: dx = 1; dy = 0; break;
		case 1: dx = 1; dy = -1; break;
		case 2: dx = 0; dy = -1; break;
		case 3: dx = -1; dy = -1; break;
		case 4: dx = -1; dy = 0; break;
		case 5: dx = -1; dy = 1; break;
		case 6: dx = 0; dy = 1; break;
		case 7: dx = 1; dy = 1; break;
		}

		Entity* address = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
		std::wstring stickerID = L"BASEATK" + std::to_wstring((unsigned __int64)this);

		if (shutdown == true)//사망으로 인한 강제종료
		{
			if (address != nullptr)
			{
				address->setFlashRGBA(0, 0, 0, 0);
			}
			aniUSet.erase(aniUSet.find(this));
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			setFakeX(0);
			setFakeY(0);
			return true;
		}

		switch (getTimer())
		{
		case 1:
			if(rightAtk) Player::ins()->setSpriteIndex(charSprIndex::RATK1);
			else  Player::ins()->setSpriteIndex(charSprIndex::LATK1);
			break;
		case 3:
			setFakeX(getFakeX() + 2.5 * dx);
			setFakeY(getFakeY() + 2.5 * dy);
			break;
		case 4:
			setFakeX(getFakeX() + 2.0 * dx);
			setFakeY(getFakeY() + 2.0 * dy);
			break;
		case 5:
			setFakeX(getFakeX() + 1.5 * dx);
			setFakeY(getFakeY() + 1.5 * dy);
			break;
		case 6:
			setFakeX(getFakeX() + 1.0 * dx);
			setFakeY(getFakeY() + 1.0 * dy);
			break;
		case 7:
			setFakeX(getFakeX() + 0.5 * dx);
			setFakeY(getFakeY() + 0.5 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() + 2 * dx);
				address->setFakeY(address->getIntegerFakeY() + 2 * dy);
			}
			attack(atkTarget.x, atkTarget.y);
			new Sticker(false, getX() + (16 * (atkTarget.x - getGridX())), getY() + (16 * (atkTarget.y - getGridY())), spr::effectCut1, 0, stickerID, true);
			if (rightAtk)
			{
				Player::ins()->setSpriteIndex(charSprIndex::RATK2);
				rightAtk = false;
			}
			else
			{
				Player::ins()->setSpriteIndex(charSprIndex::LATK2);
				rightAtk = true;
			}
			break;
		case 9:
			setFakeX(getFakeX() - 0.5 * dx);
			setFakeY(getFakeY() - 0.5 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			break;
		case 10:
			setFakeX(getFakeX() - 1.0 * dx);
			setFakeY(getFakeY() - 1.0 * dy);
			break;
		case 11:
			setFakeX(getFakeX() - 1.5 * dx);
			setFakeY(getFakeY() - 1.5 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);
			break;
		case 12:
			setFakeX(getFakeX() - 2.0 * dx);
			setFakeY(getFakeY() - 2.0 * dy);
			break;
		case 13:
			setFakeX(getFakeX() - 2.5 * dx);
			setFakeY(getFakeY() - 2.5 * dy);
			break;
		case 15:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);
			break;
		case 17:
			delete(((Sticker*)(StickerList.find(stickerID))->second));
		case 19:
			setFakeX(0);
			setFakeY(0);
			resetTimer();
			setAniType(aniFlag::null);
			Player::ins()->setSpriteIndex(charSprIndex::WALK);
			if (entityInfo.isPlayer == true) { turnWait(endAtk()); }
			else { endAtk(); }
			return true;
		}

		if (getTimer() >= 5)
		{
			Entity* address = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
			if (address != nullptr)
			{
				Uint8 targetR, targetG, targetB, targetAlpha;
				address->getFlashRGBA(targetR, targetG, targetB, targetAlpha);
				if (address->getFlashType() == 1)
				{
					if (targetG > 51) { targetG -= 51; }
					else { targetG = 0; }
					if (targetB > 51) { targetB -= 51; }
					else { targetB = 0; }
					if (targetAlpha > 51) { targetAlpha -= 51; }
					else { targetAlpha = 0; }
					address->setFlashRGBA(targetR, targetG, targetB, targetAlpha);
				}
			}
		}
	}
	else if (getAniType() == aniFlag::felling)
	{
		addTimer();

		char dx;
		char dy;

		switch (entityInfo.direction)
		{
		case 0: dx = 1; dy = 0; break;
		case 1: dx = 1; dy = -1; break;
		case 2: dx = 0; dy = -1; break;
		case 3: dx = -1; dy = -1; break;
		case 4: dx = -1; dy = 0; break;
		case 5: dx = -1; dy = 1; break;
		case 6: dx = 0; dy = 1; break;
		case 7: dx = 1; dy = 1; break;
		}

		Prop* address = TileProp(PlayerX()+dx, PlayerY()+dy, PlayerZ());
		std::wstring stickerID = L"BASEATK" + std::to_wstring((unsigned __int64)this);

		if (shutdown == true)//사망으로 인한 강제종료
		{
			aniUSet.erase(aniUSet.find(this));
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			setFakeX(0);
			setFakeY(0);
			return true;
		}

		switch (getTimer())
		{
		case 1:
			Player::ins()->setSpriteIndex(charSprIndex::MINING1);
			break;
		case 3:
			setFakeX(getFakeX() + 2.5 * dx);
			setFakeY(getFakeY() + 2.5 * dy);
			break;
		case 4:
			setFakeX(getFakeX() + 2.0 * dx);
			setFakeY(getFakeY() + 2.0 * dy);
			break;
		case 5:
			setFakeX(getFakeX() + 1.5 * dx);
			setFakeY(getFakeY() + 1.5 * dy);
			break;
		case 6:
			setFakeX(getFakeX() + 1.0 * dx);
			setFakeY(getFakeY() + 1.0 * dy);
			break;
		case 7:
			setFakeX(getFakeX() + 0.5 * dx);
			setFakeY(getFakeY() + 0.5 *dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() + 2 * dx);
				address->setFakeY(address->getIntegerFakeY() + 2 * dy);
			}
			Player::ins()->setSpriteIndex(charSprIndex::MINING2);
			address->leadItem.propHP -= 180;
			address->displayHPBarCount = 100;
			address->alphaHPBar = 255;
			address->alphaFakeHPBar = 255;

			if (address->leadItem.propHP <= 0)
			{
				if (address->leadItem.checkFlag(itemFlag::STUMP) == false)
				{
					addAniUSetPlayer(address, aniFlag::treeFalling);
					address->displayHPBarCount = 50;
					address->leadItem.eraseFlag(itemFlag::PROP_BLOCKER);
					Player::ins()->updateVision(Player::ins()->entityInfo.eyeSight);
				}
				else
				{
					address->displayHPBarCount = 50;
				}
			}

			new Sticker(false, getX() + (16 * dx), getY() + (16 * dy), spr::effectCut1, 0, stickerID, true);
			break;
		case 9:
			setFakeX(getIntegerFakeX() - 0.5 * dx);
			setFakeY(getIntegerFakeY() - 0.5 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			break;
		case 10:
			setFakeX(getFakeX() - 1.0 * (float)dx);
			setFakeY(getIntegerFakeY() - 1.0 * (float)dy);
			break;
		case 11:
			setFakeX(getFakeX() - 1.5 * dx);
			setFakeY(getFakeY() - 1.5 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);
			break;
		case 12:
			setFakeX(getFakeX() - 2.0 * (float)dx);
			setFakeY(getFakeY() - 2.0 * (float)dy);
			break;
		case 13:
			setFakeX(getFakeX() - 2.5 * dx);
			setFakeY(getFakeY() - 2.5 * dy);
			break;
		case 15:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);
			break;
		case 17:
			delete(((Sticker*)(StickerList.find(stickerID))->second));
		case 19:
			setFakeX(0);
			setFakeY(0);
			resetTimer();
			setAniType(aniFlag::null);
			Player::ins()->setSpriteIndex(charSprIndex::WALK_2H);
			if (entityInfo.isPlayer == true) { turnWait(1.0); }
			else {  }
			return true;
		}
	}
	else if (getAniType() == aniFlag::miningWall)
	{
		addTimer();

		char dx;
		char dy;

		switch (entityInfo.direction)
		{
		case 0: dx = 1; dy = 0; break;
		case 1: dx = 1; dy = -1; break;
		case 2: dx = 0; dy = -1; break;
		case 3: dx = -1; dy = -1; break;
		case 4: dx = -1; dy = 0; break;
		case 5: dx = -1; dy = 1; break;
		case 6: dx = 0; dy = 1; break;
		case 7: dx = 1; dy = 1; break;
		}

		std::wstring stickerID = L"BASEATK" + std::to_wstring((unsigned __int64)this);

		if (shutdown == true)//사망으로 인한 강제종료
		{
			aniUSet.erase(aniUSet.find(this));
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			setFakeX(0);
			setFakeY(0);
			return true;
		}

		TileData& tile = World::ins()->getTile(PlayerX() + dx, PlayerY() + dy, PlayerZ());

		switch (getTimer())
		{
		case 1:
			Player::ins()->setSpriteIndex(charSprIndex::MINING1);
			break;
		case 3:
			setFakeX(getFakeX() + 2.5 * dx);
			setFakeY(getFakeY() + 2.5 * dy);
			break;
		case 4:
			setFakeX(getFakeX() + 2.0 * dx);
			setFakeY(getFakeY() + 2.0 * dy);
			break;
		case 5:
			setFakeX(getFakeX() + 1.5 * dx);
			setFakeY(getFakeY() + 1.5 * dy);
			break;
		case 6:
			setFakeX(getFakeX() + 1.0 * dx);
			setFakeY(getFakeY() + 1.0 * dy);
			break;
		case 7:
			setFakeX(getFakeX() + 0.5 * dx);
			setFakeY(getFakeY() + 0.5 * dy);
			Player::ins()->setSpriteIndex(charSprIndex::MINING2);
			tile.wallHP -= 40;
			tile.displayHPBarCount = 100;
			tile.alphaHPBar = 255;
			tile.alphaFakeHPBar = 255;

			if (tile.wallHP <= 0)
			{
				tile.displayHPBarCount = 50;

				ItemStack* itemPtr = nullptr;

				if (TileWall(PlayerX() + dx, PlayerY() + dy, PlayerZ()) == itemVIPCode::dirtWall)
				{
					if(randomRange(0,100)<=25) itemPtr = new ItemStack(PlayerX() + dx, PlayerY() + dy, PlayerZ(), { {396,1} });
					for (int i = 0; i < 8; i++)
					{
						new Particle(getX() + 16 * dx + randomRange(-3, 3), getY() + 16 * dy + 4 + randomRange(-3, 3), randomRange(24, 31), randomRangeFloat(-1.2, 1.2), randomRangeFloat(-2.6, -3.2), 0.18, randomRange(25, 35));
					}
				}
				else if (TileWall(PlayerX() + dx, PlayerY() + dy, PlayerZ()) == itemVIPCode::stoneWall)
				{
					if (randomRange(0, 100) <= 25) itemPtr = new ItemStack(PlayerX() + dx, PlayerY() + dy, PlayerZ(), { {398,1} });
					for (int i = 0; i < 8; i++)
					{
						new Particle(getX() + 16 * dx + randomRange(-3, 3), getY() + 16 * dy + 4 + randomRange(-3, 3), randomRange(8, 15), randomRangeFloat(-1.2, 1.2), randomRangeFloat(-2.6, -3.2), 0.18, randomRange(25, 35));
					}
				}
				else
				{
					for (int i = 0; i < 8; i++)
					{
						new Particle(getX() + 16 * dx + randomRange(-3, 3), getY() + 16 * dy + 4 + randomRange(-3, 3), randomRange(8, 15), randomRangeFloat(-1.2, 1.2), randomRangeFloat(-2.6, -3.2), 0.18, randomRange(25, 35));
					}
				}

				if(itemPtr != nullptr) addAniUSetPlayer(itemPtr, aniFlag::drop);

				DestroyWall(PlayerX() + dx, PlayerY() + dy, PlayerZ());
				Player::ins()->updateVision(Player::ins()->entityInfo.eyeSight);
			}
			new Sticker(false, getX() + (16 * dx), getY() + (16 * dy), spr::effectCut1, 0, stickerID, true);
			break;
		case 9:
			setFakeX(getIntegerFakeX() - 0.5 * dx);
			setFakeY(getIntegerFakeY() - 0.5 * dy);
			break;
		case 10:
			setFakeX(getFakeX() - 1.0 * (float)dx);
			setFakeY(getIntegerFakeY() - 1.0 * (float)dy);
			break;
		case 11:
			setFakeX(getFakeX() - 1.5 * dx);
			setFakeY(getFakeY() - 1.5 * dy);
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);
			break;
		case 12:
			setFakeX(getFakeX() - 2.0 * (float)dx);
			setFakeY(getFakeY() - 2.0 * (float)dy);
			break;
		case 13:
			setFakeX(getFakeX() - 2.5 * dx);
			setFakeY(getFakeY() - 2.5 * dy);
			break;
		case 15:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);
			break;
		case 17:
			delete(((Sticker*)(StickerList.find(stickerID))->second));
		case 19:
			setFakeX(0);
			setFakeY(0);
			resetTimer();
			setAniType(aniFlag::null);
			Player::ins()->setSpriteIndex(charSprIndex::WALK_2H);
			if (entityInfo.isPlayer == true) { turnWait(1.0); }
			else {}
			return true;
		}
	}
	else if (getAniType() == aniFlag::throwing)
	{
		//거리에 따라 적이 피격하는데에 걸리는 시간을 길게 만들 것
		addTimer();

		char dx;
		char dy;

		switch (entityInfo.direction)
		{
		case 0: dx = 1; dy = 0; break;
		case 1: dx = 1; dy = -1; break;
		case 2: dx = 0; dy = -1; break;
		case 3: dx = -1; dy = -1; break;
		case 4: dx = -1; dy = 0; break;
		case 5: dx = -1; dy = 1; break;
		case 6: dx = 0; dy = 1; break;
		case 7: dx = 1; dy = 1; break;
		}

		Entity* address = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
		std::wstring stickerID = L"BASEATK" + std::to_wstring((unsigned __int64)this);

		if (shutdown == true)//사망으로 인한 강제종료
		{
			if (address != nullptr)
			{
				address->setFlashRGBA(0, 0, 0, 0);
			}
			aniUSet.erase(aniUSet.find(this));
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			setFakeX(0);
			setFakeY(0);
			return true;
		}

		switch (getTimer())
		{
		case 2:
			setFakeX(getIntegerFakeX() + 3 * dx);
			setFakeY(getIntegerFakeY() + 3 * dy);
			break;
		case 3:
			setFakeX(getIntegerFakeX() + 2 * dx);
			setFakeY(getIntegerFakeY() + 2 * dy);
			break;
		case 4:
			setFakeX(getIntegerFakeX() + 1 * dx);
			setFakeY(getIntegerFakeY() + 1 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() + 2 * dx);
				address->setFakeY(address->getIntegerFakeY() + 2 * dy);
			}

			attack(atkTarget.x, atkTarget.y);
			new Sticker(false, getX() + (16 * (atkTarget.x - getGridX())), getY() + (16 * (atkTarget.y - getGridY())), spr::effectCut1, 0, stickerID, true);
			break;
		case 5:
			setFakeX(getIntegerFakeX() - 1 * dx);
			setFakeY(getIntegerFakeY() - 1 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			break;
		case 6:
			setFakeX(getIntegerFakeX() - 2 * dx);
			setFakeY(getIntegerFakeY() - 2 * dy);
			if (address != nullptr)
			{
				address->setFakeX(address->getIntegerFakeX() - 1 * dx);
				address->setFakeY(address->getIntegerFakeY() - 1 * dy);
			}
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);
			break;
		case 7:
			setFakeX(getIntegerFakeX() - 3 * dx);
			setFakeY(getIntegerFakeY() - 3 * dy);
			break;
		case 8:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);
			break;
		case 10:
			delete(((Sticker*)(StickerList.find(stickerID))->second));
		case 20:
			setFakeX(0);
			setFakeY(0);
			resetTimer();
			setAniType(aniFlag::null);
			if (entityInfo.isPlayer == true) { turnWait(endAtk()); }
			else { endAtk(); }
			return true;
		}

		if (getTimer() >= 5)
		{
			Entity* address = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
			if (address != nullptr)
			{
				Uint8 targetR, targetG, targetB, targetAlpha;
				address->getFlashRGBA(targetR, targetG, targetB, targetAlpha);
				if (address->getFlashType() == 1)
				{
					if (targetG > 51) { targetG -= 51; }
					else { targetG = 0; }
					if (targetB > 51) { targetB -= 51; }
					else { targetB = 0; }
					if (targetAlpha > 51) { targetAlpha -= 51; }
					else { targetAlpha = 0; }
					address->setFlashRGBA(targetR, targetG, targetB, targetAlpha);
				}
			}
		}
	}
	else if (getAniType() == aniFlag::shotSingle)
	{
		addTimer();
		static Bullet* bulletPtr = nullptr;

		int dx, dy;
		dir2Coord(entityInfo.direction, dx, dy);

		Entity* ePtr = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
		std::wstring stickerID = L"BASEATK" + std::to_wstring((unsigned __int64)this);

		if (shutdown == true)//사망으로 인한 강제종료
		{
			if (ePtr != nullptr) ePtr->setFlashRGBA(0, 0, 0, 0);
			aniUSet.erase(aniUSet.find(this));
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			setFakeX(0);
			setFakeY(0);
			return true;
		}

		float spd = 7.0;
		float delX = 16.0 * (atkTarget.x - getGridX());
		float delY = 16.0 * (atkTarget.y - getGridY());
		float dist = std::sqrt(std::pow(delX, 2) + std::pow(delY, 2));
		float cosVal = delX / dist;
		float sinVal = delY / dist;
		float xSpd = spd * cosVal;
		float ySpd = spd * sinVal;
		static int hitTimer = -1;
		if (getTimer() == 1)
		{
			bulletPtr = new Bullet(getX(), getY());
			bulletPtr->sprite = spr::bulletset;
			bulletPtr->sprIndex = 0 + del2Dir(delX, delY);
		}

		if (bulletPtr != nullptr)
		{
			bulletPtr->addFakeX(xSpd);
			bulletPtr->addFakeY(ySpd);


			std::wprintf(L"bullet fake X:%f, fake Y:%f, delX :%f, delY: %f\n", bulletPtr->getFakeX(), bulletPtr->getFakeY(), delX, delY);
			if (std::fabs(bulletPtr->getFakeX()) >= std::fabs(delX) && std::fabs(bulletPtr->getFakeY()) >= std::fabs(delY))
			{
				delete bulletPtr;
				bulletPtr = nullptr;
				hitTimer = getTimer();
			}
		}


		switch (getTimer())
		{
		case 2:

			setFakeX(getIntegerFakeX() - 2 * dx);
			setFakeY(getIntegerFakeY() - 2 * dy);
			break;
		case 3:
			setFakeX(getIntegerFakeX() - 1 * dx);
			setFakeY(getIntegerFakeY() - 1 * dy);
			break;
		case 5:
			setFakeX(getIntegerFakeX() + 1 * dx);
			setFakeY(getIntegerFakeY() + 1 * dy);
			break;
		case 6:
			setFakeX(getIntegerFakeX() + 2 * dx);
			setFakeY(getIntegerFakeY() + 2 * dy);
			break;
		}

		if (bulletPtr == nullptr)
		{
			if (getTimer() == hitTimer)
			{
				if (ePtr != nullptr)
				{
					ePtr->setFakeX(ePtr->getIntegerFakeX() + 2 * dx);
					ePtr->setFakeY(ePtr->getIntegerFakeY() + 2 * dy);
				}
				attack(atkTarget.x, atkTarget.y);
				new Sticker(false, getX() + (16 * (atkTarget.x - getGridX())), getY() + (16 * (atkTarget.y - getGridY())), spr::effectCut1, 0, stickerID, true);
			}
			else if (getTimer() == hitTimer + 1)
			{
				if (ePtr != nullptr)
				{
					ePtr->setFakeX(ePtr->getIntegerFakeX() - 1 * dx);
					ePtr->setFakeY(ePtr->getIntegerFakeY() - 1 * dy);
				}
			}
			else if (getTimer() == hitTimer + 2)
			{
				if (ePtr != nullptr)
				{
					ePtr->setFakeX(ePtr->getIntegerFakeX() - 1 * dx);
					ePtr->setFakeY(ePtr->getIntegerFakeY() - 1 * dy);
				}
				((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);
			}
			else if (getTimer() == hitTimer + 4)
			{
				((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);
			}
			else if (getTimer() == hitTimer + 6)
			{
				delete(((Sticker*)(StickerList.find(stickerID))->second));
			}
			else if (getTimer() == hitTimer + 8)
			{
				setFakeX(0);
				setFakeY(0);
				resetTimer();
				setAniType(aniFlag::null);

				if (Aim::ins() != nullptr)
				{
					Aim::ins()->close(aniFlag::null);
				}

				if (entityInfo.isPlayer == true) { turnWait(endAtk()); }
				else { endAtk(); }
				return true;
			}


			if (getTimer() >= hitTimer)
			{
				Entity* ePtr = TileEntity(atkTarget.x, atkTarget.y, atkTarget.z);
				if (ePtr != nullptr)
				{
					Uint8 targetR, targetG, targetB, targetAlpha;
					ePtr->getFlashRGBA(targetR, targetG, targetB, targetAlpha);
					if (ePtr->getFlashType() == 1)
					{
						if (targetG > 51) { targetG -= 51; }
						else { targetG = 0; }
						if (targetB > 51) { targetB -= 51; }
						else { targetB = 0; }
						if (targetAlpha > 51) { targetAlpha -= 51; }
						else { targetAlpha = 0; }
						ePtr->setFlashRGBA(targetR, targetG, targetB, targetAlpha);
					}
				}
			}
		}
	}
	else if (getAniType() == aniFlag::fireStorm)
	{
		addTimer();

		std::wstring stickerID = L"FIRESTORM";

		switch (getTimer())
		{
		case 1:
			new Sticker(false, getX() + (16 * (getSkillTarget().x - getGridX())), getY() + (16 * (getSkillTarget().y - getGridY())), spr::fireStorm, 0, stickerID, true);
			new Flame(getSkillTarget().x, getSkillTarget().y, getGridZ(), flameFlag::BIG);


			break;
		case 5:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(1);

			new Flame(getSkillTarget().x + 1, getSkillTarget().y, getGridZ(), flameFlag::NORMAL);
			new Flame(getSkillTarget().x - 1, getSkillTarget().y, getGridZ(), flameFlag::NORMAL);
			new Flame(getSkillTarget().x, getSkillTarget().y + 1, getGridZ(), flameFlag::NORMAL);
			new Flame(getSkillTarget().x, getSkillTarget().y - 1, getGridZ(), flameFlag::NORMAL);


			break;
		case 9:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(2);

			new Flame(getSkillTarget().x, getSkillTarget().y - 2, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x - 1, getSkillTarget().y - 1, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x - 2, getSkillTarget().y, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x - 1, getSkillTarget().y + 1, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x, getSkillTarget().y + 2, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x + 1, getSkillTarget().y + 1, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x + 2, getSkillTarget().y, getGridZ(), flameFlag::SMALL);
			new Flame(getSkillTarget().x + 1, getSkillTarget().y - 1, getGridZ(), flameFlag::SMALL);
			break;
		case 13:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(3);
			break;
		case 17:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(4);
			break;
		case 21:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(5);
			break;
		case 25:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(6);
			break;
		case 29:
			((Sticker*)(StickerList.find(stickerID))->second)->setSpriteIndex(7);
			break;
		case 33:
			delete(((Sticker*)(StickerList.find(stickerID))->second));
			resetTimer();
			setAniType(aniFlag::null);
			return true;
			break;
		}
	}

	return false;
}