#pragma once
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
