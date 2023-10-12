#pragma once
#include <vector>

#include "objects/CellState.h"
#include "objects/resource/ResourceEntity.h"
#include "utils/defines.h"
#include "utils/Flags.h"

//enum class CellState : char;

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

	void setEscapeThrough(int val);
	Urho3D::Vector2 getDirectionFrom(const Urho3D::Vector3& position, Urho3D::Vector2 centerEscape);

	void setNeightOccupied(unsigned char index);
	void setNeightFree(unsigned char index);
	bool ifNeightIsFree(unsigned char index) const { return !(isNeightOccupied & Flags::bitFlags[index]); };

	float getCost(const unsigned char index) const { return costToNeight[index]; }
	void setCost(const unsigned char index, float cost) { costToNeight[index] = cost; }

	bool allNeightOccupied() const { return isNeightOccupied == 255; }
	bool anyNeightFree() const { return isNeightOccupied < 255; }
	void resetNeight() { isNeightOccupied = 255; }

	char getAdditionalInfo() const { return additionalInfo; }
	int getEscapeBucket() const { return escapeBucketIndex; }

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
private:
	CellState state;
	char size, additionalInfo{};

	unsigned char isNeightOccupied = 255; //na poczatku wszystko zajete
	int escapeBucketIndex = -1;//TODO moze zrezygnowac z tego ca³kiem
	float costToNeight[8] = {0.f};
	float resourceBonuses[MAX_PLAYERS][RESOURCES_SIZE];
	//float cost{};


	bool belowCellLimit() const;
};
