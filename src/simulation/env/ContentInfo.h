#pragma once
struct content_info
{
	content_info() {
		reset();
	}

	void reset() {
		allyNumber = 0;
		enemiesNumber = 0;
		allNumber = 0;
		allyBuilding = 0;
		enemyBuilding = 0;
		allBuildingNumber = 0;
		resourceNumber = 0;
	}

	bool hasUnit() {
		return allyNumber > 0 || enemiesNumber > 0 || allNumber > 0;
	}

	bool hasBuilding() {
		return allBuildingNumber > 0;
	}

	bool hasResource() {
		return resourceNumber > 0;
	}

	int allyNumber;
	int enemiesNumber;
	int allNumber;

	int allyBuilding;
	int enemyBuilding;
	int allBuildingNumber;

	int resourceNumber;
};
