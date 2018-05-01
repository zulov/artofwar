#pragma once
#include "ObjectEnums.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/Vector2.h>
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
	void createBox(float bucketSize);
	void setCenter(float _centerX, float _centerY);
	Urho3D::Vector2& getCenter();

	std::vector<std::pair<int, float>>& getNeightbours();
	std::vector<std::pair<int, float>>& getOccupiedNeightbours();
	char getAdditonalInfo() const { return additonalInfo; }
	bool isUnit();
	void setEscapeThrought(int val);
	Urho3D::Vector2* getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket);
	int getEscapeBucket() const { return escapeBucketIndex; }

private:
	ObjectType type;
	Urho3D::Vector2 center;
	Urho3D::Node* box;
	Urho3D::StaticModel* model;
	Static* object{};
	char additonalInfo{};
	std::vector<std::pair<int, float>> neighbours;
	std::vector<std::pair<int, float>> occupiedNeightbours;
	float cost{};
	int escapeBucketIndex = -1;
};
