#pragma once
enum class AiActionType:char {
	WORKER_CREATE=0,
	UNIT_CREATE,
	BUILDING_CREATE
};

enum class AiUnitType:char {
	INFANTRY=0,
	ARCHERY,
	CAVALRY 
};

enum class AiCommonMetric:char {
	COST_SUM=0,
	MAX_HP,
	ARMOUR 
};

enum class AiInfantryMetric:char {
	CLOSE_ATTACK=0,
	CLOSE_ATTACK_RELOAD,
	ARMOUR 
};
