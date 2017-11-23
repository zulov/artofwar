#pragma once
#include <vector>
#include "objects/unit/Unit.h"
#include <Urho3D/Graphics/StaticModel.h>
#include "ComplexBucketData.h"
#include "simulation/env/ContentInfo.h"

class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Unit*>& getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	void setCenter(double _centerX, double _centerY);
	Vector2& getCenter();

	ObjectType getType();
	void setStatic(Static* object);
	void removeStatic();
	void createBox(double bucketSize);
	Vector3* getDirectrionFrom(Vector3* position);

	void upgrade(ComplexBucketData* _data);
	void setNeightbours(std::vector<std::pair<int, float>*>* tempNeighbour);
	std::vector<std::pair<int, float>*>& getNeightbours();
	int &getSize();
	bool incUnitsPerPlayer(content_info* ci);
	void update(content_info* ci);
private:
	int size;
	int unitsNumberPerPlayer[MAX_PLAYERS];
	std::vector<Unit*> content;
	ComplexBucketData* data;
};
