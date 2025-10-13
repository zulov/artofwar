#pragma once

enum class UnitAction : char {
	GO = 0,
	STOP,
	CHARGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT
};

enum class UnitActionType : char {
	ORDER = 0,
	FORMATION
};

enum class UnitOrderType :char {
	GO,
	STOP,
	CHARGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT
};
