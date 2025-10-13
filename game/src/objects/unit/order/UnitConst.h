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

// const char* ORDER_NAME_KEYS[] = {
// 	"ord_go", "ord_stop", "ord_charge", "ord_attack", "ord_dead", "ord_defend", "ord_follow", "ord_collet"
// };
//
// const char* ORDER_ICONS[] = {
// 	"ord_go.png", "ord_stop", "ord_charge", "ord_attack", "ord_dead", "ord_defend", "ord_follow", "ord_collet"
// };
