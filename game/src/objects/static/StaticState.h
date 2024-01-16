#pragma once
enum class StaticState : char {
	CREATING=-1,
	ALIVE,
	FREE,//TODO czy to potrzebne?
	DEAD,
	DISPOSE
};
