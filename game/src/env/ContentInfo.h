#pragma once
#include <algorithm>
#include "database/DatabaseCache.h"
#include "utils/defines.h"


struct content_info {
	content_info() {
		resourceNumber = new unsigned char[Game::getDatabase()->getResourcesSize()];
		reset();
	}

	~content_info() {
		delete[] resourceNumber;
	}

	content_info(const content_info&) = delete;

	void reset() {
		allyNumber = 0;
		enemiesNumber = 0;
		allyBuilding = 0;
		enemyBuilding = 0;
		std::fill_n(unitsNumberPerPlayer, MAX_PLAYERS, 0);
		std::fill_n(buildingNumberPerPlayer, MAX_PLAYERS, 0);
		std::fill_n(resourceNumber, Game::getDatabase()->getResourcesSize(), 0);

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
		return std::max_element(resourceNumber, resourceNumber + Game::getDatabase()->getResourcesSize()) -
			resourceNumber;
	}

	unsigned short unitsNumberPerPlayer[MAX_PLAYERS];
	unsigned short allyNumber;
	unsigned short enemiesNumber;

	unsigned char allyBuilding;
	unsigned char enemyBuilding;
	unsigned char buildingNumberPerPlayer[MAX_PLAYERS];

	unsigned char *resourceNumber;

	bool hasUnit;
	bool hasBuilding;
	bool hasResource;
};
