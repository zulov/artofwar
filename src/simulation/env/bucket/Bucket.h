#pragma once
#include <vector>
#include "objects/unit/Unit.h"
#include <Urho3D/Graphics/StaticModel.h>
#include "ComplexBucketData.h"

class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Unit*>* getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	void setCenter(double _centerX, double _centerY);
	Vector2& getCenter();

	ObjectType getType();
	void setStatic(Static* object);
	void removeStatic();
	void createBox(double bucketSize);
	Vector3* getDirectrionFrom(Vector3* position);

	void upgrade();
	void setNeightbours(std::vector<std::pair<int, float>*>* tempNeighbour);
	std::vector<std::pair<int, float>*>& getNeightbours();
private:

	std::vector<Unit*>* content;
	ComplexBucketData* data;
};
