#pragma once
#include <vector>

#include "objects/CellState.h"
#include "objects/resource/ResourceEntity.h"
#include "utils/defines.h"
#include "utils/Flags.h"

namespace Urho3D {
	class Vector2;
	class Vector3;
}

class Building;
class Static;

class ComplexBucketData {
public:
	ComplexBucketData();
	~ComplexBucketData() = default;
	ComplexBucketData(const ComplexBucketData&) = delete;

	void setStatic(Static* object);
	void clear();

	Urho3D::Vector2 getDirectionFrom(const Urho3D::Vector3& position, Urho3D::Vector2 centerEscape);

	void setNeightOccupied(unsigned char index);
	void setNeightFree(unsigned char index);
	bool ifNeightIsFree(unsigned char index) const { return !(isNeightOccupied & Flags::bitFlags[index]); }

	float getCost() const;

	bool allNeightOccupied() const { return isNeightOccupied == 255; }
	bool allNeightFree() const { return isNeightOccupied == 0; }
	bool anyNeightFree() const { return isNeightOccupied != 255; }
	void setAllOccupied() { isNeightOccupied = 255; }

	char getAdditionalInfo() const { return additionalInfo; }

	CellState getType() const { return state; }

	void updateSize(char val, CellState cellState);

	void setResBonuses(char player, const std::vector<char>& resIds, float bonus);
	void resetResBonuses(char player, char resId);
	float getResBonus(char player, short resId) const;

	void setDeploy();
	bool cellIsCollectable() const;
	bool cellIsAttackable() const;
	bool isPassable() const { return state <= CellState::DEPLOY; }
	bool isBuildable() const;
	void setGradient(short gradient) { this->gradient = gradient; }
	short getGradient() const { return gradient; }

	void setIndexCloseIndexes(unsigned char indexOfCloseIndexes, unsigned char indexOfSecondCloseIndexes) {
		this->indexOfCloseIndexes = indexOfCloseIndexes;
		this->indexOfSecondCloseIndexes = indexOfSecondCloseIndexes;
	}

	unsigned char getIndexOfCloseIndexes() const { return indexOfCloseIndexes; }
	unsigned char getIndexSecondOfCloseIndexes() const { return indexOfSecondCloseIndexes; }

private:
	CellState state;
	char size, additionalInfo{};

	unsigned char isNeightOccupied = 0; //na poczatku wszystko wolne
	unsigned char indexOfCloseIndexes;
	unsigned char indexOfSecondCloseIndexes;
	short gradient = -1;
	float cost = 0.f;
	float resourceBonuses[MAX_PLAYERS][RESOURCES_SIZE];

	bool belowCellLimit() const;
};