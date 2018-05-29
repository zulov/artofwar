#pragma once

#define UNIT_ORDER_SIZE 8

enum class UnitOrder : char
{
	GO = 0,
	STOP,
	CHARGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT
};
