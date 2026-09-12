#pragma once
enum class CellState : unsigned char {
	NONE=0,
	ATTACK,
	COLLECT,
	
	DEPLOY,

	RESOURCE,
	BUILDING
};
