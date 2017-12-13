#pragma once
#include <vector>
#include "objects/unit/Unit.h"
#include <Urho3D/Graphics/StaticModel.h>
#include "simulation/env/ContentInfo.h"

class ComplexBucketData;

class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Unit*>& getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	Vector2& getCenter();

	void setStatic(Static* object);
	void removeStatic();
	Vector3* getDirectrionFrom(Vector3* position);

	void upgrade(ComplexBucketData* _data);
	std::vector<std::pair<int, float>*>& getNeightbours();
	int &getSize();
	bool incUnitsPerPlayer(content_info* ci, int activePlayer, bool checks[]);
	void update(content_info* ci, bool checks[], int activePlayer);
private:
	int size;
	int unitsNumberPerPlayer[MAX_PLAYERS];
	std::vector<Unit*> content;
	ComplexBucketData* data;
};
