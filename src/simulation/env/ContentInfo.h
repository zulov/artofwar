#pragma once
#include <algorithm>

struct content_info
{
	content_info() {
		reset();
	}

	void reset() {
		allyNumber = 0;
		enemiesNumber = 0;
		std::fill_n(unitsNumberPerPlayer, MAX_PLAYERS, 0);
		allyBuilding = 0;
		enemyBuilding = 0;
		std::fill_n(buildingNumberPerPlayer, MAX_PLAYERS, 0);
		resourceNumber = 0;

		hasBuilding = false;
		hasUnit = false;
	}


	bool hasResource() {
		return resourceNumber > 0;
	}

	char biggestBuilding() {
		return std::max_element(buildingNumberPerPlayer, buildingNumberPerPlayer + MAX_PLAYERS) -
			buildingNumberPerPlayer;
	}

	char biggestUnits() {
		return std::max_element(unitsNumberPerPlayer, unitsNumberPerPlayer + MAX_PLAYERS) -
			unitsNumberPerPlayer;
	}

	int unitsNumberPerPlayer[MAX_PLAYERS];
	int allyNumber;
	int enemiesNumber;
	bool hasUnit;

	int allyBuilding;
	int enemyBuilding;
	int buildingNumberPerPlayer[MAX_PLAYERS];
	bool hasBuilding;

	int resourceNumber;
};
