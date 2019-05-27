#pragma once
#include "objects/CellState.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/Vector2.h>



class Building;
class Static;

struct CostPair {
	int index;
	float cost;

	CostPair(int index, float cost)
		: index(index), cost(cost) {
	}
};


class ComplexBucketData {
public:
	ComplexBucketData();
	~ComplexBucketData();

	void setStatic(Static* _object);
	void removeStatic();

	void setCenter(float _centerX, float _centerY);

	void setEscapeThrought(int val);
	Urho3D::Vector2* getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket);

	void setNeightOccupied(const unsigned char index);
	void setNeightFree(const unsigned char index);
	bool ifNeightIsFree(const unsigned char index) const;

	float getCost(const unsigned char index) { return costToNeight[index]; }
	void setCost(const unsigned char index, float cost) { costToNeight[index] = cost; }

	bool allNeightOccupied() const {return isNeightOccupied == 255;}

	bool allNeightFree() const { return isNeightOccupied == 0; }

	Urho3D::Vector2& getCenter() { return center; }
	char getAdditionalInfo() const { return additionalInfo; }
	int getEscapeBucket() const { return escapeBucketIndex; }
	bool isUnit() const { return state < CellState::NONE; }
	CellState getType() const { return state; }
	char getSize() { return size; }
	void updateSize(char val, CellState cellState);
	bool belowCellLimit();
	void setDeploy(Building* building);
	void removeDeploy();
private:
	CellState state;
	char size, additionalInfo{};

	unsigned char isNeightOccupied = 0;
	float costToNeight[8]={0,0,0,0,0,0,0,0};

	//float cost{};
	int escapeBucketIndex = -1;
	Urho3D::Vector2 center;

	Static* object{};
};
