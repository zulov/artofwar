#pragma once

enum class UnitAction :unsigned char {
	GO = 0,
	STOP,
	CHARGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT
};

enum class UnitActionType :unsigned char {
	ORDER = 0,
	FORMATION
};

enum class UnitOrderType :unsigned char {
	GO = 0,
	STOP,
	CHARGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT
};
