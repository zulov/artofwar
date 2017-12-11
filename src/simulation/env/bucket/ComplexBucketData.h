#pragma once
#include "ObjectEnums.h"
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <vector>
#include "simulation/env/ContentInfo.h"

class Static;

class ComplexBucketData
{
public:
	ComplexBucketData();
	~ComplexBucketData();

	ObjectType getType() const { return type; };
	void setStatic(Static* object);
	void removeStatic();
	void createBox(double bucketSize);
	Urho3D::Vector3* getDirectrionFrom(Urho3D::Vector3* position);
	void setCenter(double _centerX, double _centerY);
	Urho3D::Vector2& getCenter();
	void setNeightbours(std::vector<std::pair<int, float>*>* tempNeightbours);
	std::vector<std::pair<int, float>*>& getNeightbours();
	char getAdditonalInfo();
	bool isUnit();
private:
	ObjectType type;
	Urho3D::Vector2 center;
	Urho3D::Node* box;
	Urho3D::StaticModel* model;
	Static* object;
	short additonalInfo;
	std::vector<std::pair<int, float>*> neighbour;
	float cost;
};
