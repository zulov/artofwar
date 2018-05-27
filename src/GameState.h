#pragma once
#define GAME_STATE_SIZE 7
enum class GameState : char
{
	STARTING=0,
	NEW_GAME,
	LOADING,
	MENU_MAIN,
	RUNNING,
	PAUSE,
	CLOSING
};
