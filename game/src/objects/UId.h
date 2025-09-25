#include "ObjectEnums.h"
#include <assert.h>

constexpr unsigned PLAYER_MASK = 0b11100000000000000000000000000000;
constexpr unsigned TYPE_MASK = 0b00011000000000000000000000000000;

struct UId {
	const unsigned v;

	explicit UId(unsigned v) : v(v) {}

	unsigned getId() const { return v; }

	char getPlayer() const {
		return getType() == ObjectType::RESOURCE ? -1 : (v & PLAYER_MASK) >> 29;
	}

	ObjectType getType() const {
		auto type = (v & TYPE_MASK) >> 27;
		assert(type == 0 || type == 1 || type == 2);
		return static_cast<ObjectType>(type);
	}

	static unsigned create(ObjectType type, char player = -1) {
		assert(type != ObjectType::NONE);
		unsigned v = static_cast<char>(type);
		return (((player < 0 ? 0 : player) << 2) + v) << 27;
	}
};

// 000_00 000 00000000 00000000 00000000 
// 3  player
// 2  bits type id
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

// R	00000000 00000000 00000000 00000000 0
// U0	00001000 00000000 00000000 00000000 134217728
// U1	00101000 00000000 00000000 00000000 671088640
// B0	00010000 00000000 00000000 00000000 268435456
// B1	00110000 00000000 00000000 00000000 805306368
