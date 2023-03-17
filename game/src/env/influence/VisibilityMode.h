#pragma once
#include <magic_enum.hpp>

enum class VisibilityMode:char {
	PLAYER_ONLY= 0,
	ALL_PLAYERS,
	//TEAM,
	ALL,
};


constexpr inline VisibilityMode NEXT_VISIBILITY_MODES[magic_enum::enum_count<VisibilityMode>()] = {
	VisibilityMode::ALL_PLAYERS, VisibilityMode::ALL, VisibilityMode::PLAYER_ONLY
};
