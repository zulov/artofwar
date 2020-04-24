#pragma once

#define RAND_FLOAT_SIZE 3
#define RAND_INT_SIZE 3

enum class RandFloatType : char {
	AI=0,
	RESOURCE_ROTATION,
	COLLISION_FORCE
};

enum class RandIntType : char {
	SAVE=0,
	PLAYER_NAME,
	RESOURCE_NODE
};
