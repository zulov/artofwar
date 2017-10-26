#pragma once
#include "ObjectEnums.h"
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <vector>

class Static;

class ComplexBucketData
{
public:
	ComplexBucketData();
	~ComplexBucketData();

	ObjectType getType();
	void setStatic(Static* object);
	void removeStatic();
	void createBox(double bucketSize);
	Urho3D::Vector3* getDirectrionFrom(Urho3D::Vector3* position);
	void setCenter(double _centerX, double _centerY);
	Urho3D::Vector2 getCenter();
	void setNeightbours(std::vector<int>* tempNeightbours);
	std::vector<int>& getNeightbours();
private:
	Urho3D::Vector2 center;
	Urho3D::Node* box;
	Urho3D::StaticModel* model;
	Static* object;
	ObjectType type;
	std::vector<int> neighbour;
	float cost;
};
