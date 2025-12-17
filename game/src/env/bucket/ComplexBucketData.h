#pragma once
#include <vector>

#include "CellEnums.h"
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
	ComplexBucketData() = default;
	~ComplexBucketData() = default;
	ComplexBucketData(const ComplexBucketData&) = delete;

	void setStatic(Static* object);
	Static* getStatic() const { return staticObj; }
	void clear();

	void setNeightOccupied(unsigned char index);
	void setNeightFree(unsigned char index);
	bool ifNeightIsFree(unsigned char index) const { return !ifNeightIsOccupied(index); }

	bool allNeightOccupied() const { return isNeightOccupied == 255; }
	bool anyNeightOccupied() const { return isNeightOccupied > 0; }
	bool allNeightFree() const { return isNeightOccupied == 0; }
	bool anyNeightFree() const { return isNeightOccupied != 255; }

	void setAllOccupied() { isNeightOccupied = 255; }
	unsigned char getNeightOccupation() const { return isNeightOccupied; }

	int getCost() const;

	char getAdditionalInfo() const { return additionalInfo; }

	CellState getType() const { return state; }

	void incStateSize(CellState cellState);
	void decStateSize();

	void setResBonuses(char player, unsigned char resId, float bonus);
	void resetResBonuses();
	float getResBonus(char player, short resId) const;

	void setDeploy();
	bool cellIsCollectable() const;
	bool cellIsAttackable() const;
	bool isPassable() const { return state <= CellState::DEPLOY; }
	bool isBuildable() const;
	void setGradient(short gradient) { this->gradient = gradient; }
	short getGradient() const { return gradient; }

	void setIndexCloseIndexes(std::pair<unsigned char, unsigned char> indexes) {
		this->indexOfCloseIndexes = indexes.first;
		this->indexOfSecondCloseIndexes = indexes.second;
	}

	unsigned char getIndexOfCloseIndexes() const { return indexOfCloseIndexes; }
	unsigned char getIndexSecondOfCloseIndexes() const { return indexOfSecondCloseIndexes; }

private:
	bool ifNeightIsOccupied(unsigned char index) const { return isNeightOccupied & Flags::bitFlags[index]; }
	CellState state = CellState::NONE;
	char size = 0, additionalInfo = -1;

	unsigned char isNeightOccupied = 0; //na poczatku wszystko wolne
	unsigned char indexOfCloseIndexes;
	unsigned char indexOfSecondCloseIndexes;
	short gradient = -1;
	Static* staticObj{};
	float* resourceBonuses{};
	int cost = 0;
	bool belowCellLimit() const;
};
