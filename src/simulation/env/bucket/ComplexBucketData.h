#pragma once
#include "ObjectEnums.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Scene/Node.h>
#include <vector>


class Static;

class ComplexBucketData
{
public:
	ComplexBucketData();
	~ComplexBucketData();

	ObjectType getType() const { return type; }
	void setStatic(Static* _object);
	void removeStatic();
	void createBox(double bucketSize);
	Urho3D::Vector3* getDirectrionFrom(Urho3D::Vector3* position);
	void setCenter(double _centerX, double _centerY);
	Urho3D::Vector2& getCenter();
	void setNeightbours(std::vector<std::pair<int, float>>* _neightbours);
	std::vector<std::pair<int, float>>& getNeightbours();
	char getAdditonalInfo();
	bool isUnit();
	void setOccupiedNeightbours(std::vector<std::pair<int, float>>* _occupiedNeightbours);
	void setEscapeThrought(int val);
	Urho3D::Vector3* getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket);
	int getEscapeBucket();

private:
	ObjectType type;
	Urho3D::Vector2 center;
	Urho3D::Node* box;
	Urho3D::StaticModel* model;
	Static* object{};
	short additonalInfo{};
	std::vector<std::pair<int, float>> neighbours;
	std::vector<std::pair<int, float>> occupiedNeightbours;
	float cost{};
	int escapeBucketIndex = -1;
};
