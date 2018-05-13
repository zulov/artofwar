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

	void setStatic(Static* _object);
	void removeStatic();
	void createBox(float bucketSize);
	void setCenter(float _centerX, float _centerY);

	void setEscapeThrought(int val);
	Urho3D::Vector2* getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket);

	std::vector<std::pair<int, float>>& getNeightbours() { return neighbours; }
	std::vector<std::pair<int, float>>& getOccupiedNeightbours() { return occupiedNeightbours; }
	Urho3D::Vector2& getCenter() { return center; }
	char getAdditonalInfo() const { return additonalInfo; }
	int getEscapeBucket() const { return escapeBucketIndex; }
	bool isUnit() const { return type == ObjectType::UNIT; }
	ObjectType getType() const { return type; }

private:
	ObjectType type;
	Urho3D::Vector2 center;
	Urho3D::Node* box;

	Static* object{};
	char additonalInfo{};
	std::vector<std::pair<int, float>> neighbours;
	std::vector<std::pair<int, float>> occupiedNeightbours;
	float cost{};
	int escapeBucketIndex = -1;
};
