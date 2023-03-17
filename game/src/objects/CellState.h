#pragma once
enum class CellState : char {
	NONE=0,
	ATTACK,
	COLLECT,
	
	DEPLOY,

	RESOURCE,
	BUILDING
};
