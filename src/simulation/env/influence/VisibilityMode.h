#pragma once
#include <magic_enum.hpp>

enum class VisibilityMode:char {
	ALL = 0,
	//TEAM,
	ALL_PLAYER,
	PLAYER_ONLY
};


constexpr inline VisibilityMode NEXT_VISIBILITY_MODES[magic_enum::enum_count<VisibilityMode>()] = {
	VisibilityMode::ALL_PLAYER, VisibilityMode::PLAYER_ONLY, VisibilityMode::ALL
};
