export module wrapVar;

import Player;
import World;
import Entity;
import Prop;

export inline int PlayerX() { return Player::ins()->getGridX(); }
export inline int PlayerY() { return Player::ins()->getGridY(); }
export inline int PlayerZ() { return Player::ins()->getGridZ(); }

export inline int TileFloor(int x, int y, int z) { return World::ins()->getTile(x,y,z).floor;  }
export inline int TileWall(int x, int y, int z) { return World::ins()->getTile(x, y, z).wall; }
export inline Entity* TileEntity(int x, int y, int z) { return (Entity*)World::ins()->getTile(x, y, z).EntityPtr; }
export inline Prop* TileProp(int x, int y, int z) { return (Prop*)World::ins()->getTile(x, y, z).PropPtr; }






