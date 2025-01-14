#include "ObjectEnums.h"
#include <assert.h>

struct UId {
	const unsigned v;
	explicit UId(unsigned v)
		: v(v) {
	}
	unsigned getId() const { return v; }

	char getPlayer() const {
		return (v << 2) >> 29;
	}

	ObjectType getType() const {
		switch (v >> 30) {
		case 0: return ObjectType::UNIT;
		case 1: return ObjectType::BUILDING;
		case 2: return ObjectType::RESOURCE;
		default: {
			assert(false);
			return ObjectType::NONE;
		}
		}
	}

};
// 00_000 000 00000000 00000000 00000000 
// 2  bits type id
// 3  player
// 27 bits id
// type
// 3		none
// 2		resources
// 1		building
// 0		unit

// 0		player 0
// 1		player 1
// 2		player 2
// 3		player 3
// 4		player 4
// 5		player 5
// 6		player 6
// 7		player 7
