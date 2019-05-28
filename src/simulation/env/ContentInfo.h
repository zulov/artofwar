#pragma once
#include "database/db_strcut.h"
#include "defines.h"
#include <algorithm>


struct content_info {
	content_info() {
		reset();
	}

	void reset() {
		allyNumber = 0;
		enemiesNumber = 0;
		allyBuilding = 0;
		enemyBuilding = 0;
		std::fill_n(unitsNumberPerPlayer, MAX_PLAYERS, 0);
		std::fill_n(buildingNumberPerPlayer, MAX_PLAYERS, 0);
		std::fill_n(resourceNumber, RESOURCE_NUMBER_DB, 0);

		hasBuilding = false;
		hasUnit = false;
		hasResource = false;
	}


	unsigned char biggestBuilding() {
		return std::max_element(buildingNumberPerPlayer, buildingNumberPerPlayer + MAX_PLAYERS) -
			buildingNumberPerPlayer;
	}

	unsigned char biggestUnits() {
		return std::max_element(unitsNumberPerPlayer, unitsNumberPerPlayer + MAX_PLAYERS) -
			unitsNumberPerPlayer;
	}

	unsigned char biggestResource() {
		return std::max_element(resourceNumber, resourceNumber + RESOURCE_NUMBER_DB) -
			resourceNumber;
	}

	int unitsNumberPerPlayer[MAX_PLAYERS];
	int allyNumber;
	int enemiesNumber;

	int allyBuilding;
	int enemyBuilding;
	int buildingNumberPerPlayer[MAX_PLAYERS];

	int resourceNumber[RESOURCE_NUMBER_DB];

	bool hasUnit;
	bool hasBuilding;
	bool hasResource;
};
