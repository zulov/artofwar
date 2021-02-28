#pragma once
#include "objects/CellState.h"

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

	void setStatic(Static* _object);
	void clear();

	void setEscapeThrough(int val);
	Urho3D::Vector2 getDirectionFrom(Urho3D::Vector3& position, Urho3D::Vector2 centerEscape);

	void setNeightOccupied(unsigned char index);
	void setNeightFree(unsigned char index);
	bool ifNeightIsFree(unsigned char index) const;

	float getCost(const unsigned char index) const { return costToNeight[index]; }
	void setCost(const unsigned char index, float cost) { costToNeight[index] = cost; }

	bool allNeightOccupied() const { return isNeightOccupied == 255; }

	char getAdditionalInfo() const { return additionalInfo; }
	int getEscapeBucket() const { return escapeBucketIndex; }

	CellState getType() const { return state; }

	void updateSize(char val, CellState cellState);

	void setDeploy(Building* building);
	bool cellIsCollectable() const;
	bool cellIsAttackable() const;
	bool isPassable() const { return state <= CellState::DEPLOY; }
	bool isBuildable() const;
private:
	CellState state;
	char size, additionalInfo{};

	unsigned char isNeightOccupied = 255; //na poczatku wszystko okupowane
	int escapeBucketIndex = -1;
	float costToNeight[8] = {0.f};
	//float cost{};

	Static* object{};

	bool belowCellLimit() const;
};
