#include "ObjectEnums.h"

struct UId {
	const unsigned v;
	explicit UId(unsigned v)
		: v(v) {
	}
	unsigned getId() const { return v; }

	char getPlayer() const {
		switch (v >> 28) {
			
		}
	}
	
	ObjectType getType() const {
		switch (v>>28) {
		case 0:
		case 2:
		case 4:
		case 6:
		case 8:
		case 10:
		case 12:
			return ObjectType::UNIT;
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
		case 11:
		case 13:
			return ObjectType::BUILDING;
		case 14:
			return ObjectType::RESOURCE;
		default:
			assert(false);
			ObjectType::NONE;
		}
	}

};

//  4 bits type id
// 28 bits id
// type
// 15		none
// 14		resources
// 0,1		player 0 - unit, building
// 2,3		player 1 - unit, building
// 4,5		player 2 - unit, building
// 6,7		player 3 - unit, building
// 8,9		player 4 - unit, building
// 10,11	player 5 - unit, building
// 12,13	player 6 - unit, building
