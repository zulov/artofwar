#pragma once
#include "objects/CellState.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Scene/Node.h>
#include <vector>


class Static;

struct CostPair
{
	int index;
	float cost;

	CostPair(int index, float cost)
		: index(index),
		cost(cost) {
	}
};


class ComplexBucketData
{
public:
	ComplexBucketData();
	~ComplexBucketData();

	void setStatic(Static* _object);
	void removeStatic();

	void setCenter(float _centerX, float _centerY);

	void setEscapeThrought(int val);
	Urho3D::Vector2* getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket);

	std::vector<CostPair>& getNeightbours() { return neighbours; }
	std::vector<CostPair>& getOccupiedNeightbours() { return occupiedNeightbours; }
	Urho3D::Vector2& getCenter() { return center; }
	char getAdditonalInfo() const { return additonalInfo; }
	int getEscapeBucket() const { return escapeBucketIndex; }
	bool isUnit() const { return type < CellState::NONE; }
	CellState getType() const { return type; }
	char getSize() { return size; }
	void updateSize(char val, CellState cellState);
	bool belowCellLimit();

private:

	CellState type;
	char size;
	Urho3D::Vector2 center;

	Static* object{};
	char additonalInfo{};
	std::vector<CostPair> neighbours, occupiedNeightbours;
	float cost{};
	int escapeBucketIndex = -1;
};
