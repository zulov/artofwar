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

	inline ObjectType getType();
	void setStatic(Static* object);
	void removeStatic();
	void createBox(double bucketSize);
	Urho3D::Vector3* getDirectrionFrom(Urho3D::Vector3* position);
	void setCenter(double _centerX, double _centerY);
	Urho3D::Vector2& getCenter();
	void setNeightbours(std::vector<std::pair<int, float>*>* tempNeightbours);
	std::vector<std::pair<int, float>*>& getNeightbours();
	char getAdditonalInfo();
private:
	Urho3D::Vector2 center;
	Urho3D::Node* box;
	Urho3D::StaticModel* model;
	Static* object;
	ObjectType type;
	short additonalInfo;
	std::vector<std::pair<int, float>*> neighbour;
	float cost;
};
