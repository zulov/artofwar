#pragma once
#include <vector>
#include "objects/unit/Unit.h"
#include <Urho3D/Graphics/StaticModel.h>

class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Unit*> *getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	ObjectType getType();
	void setStatic(Static* object);
	void removeStatic();
	void setCenter(double _centerX, double _centerY);
	void createBox(double bucketSize);
	Vector3* getDirectrionFrom(Vector3* position);
private:
	Node * box;
	StaticModel* model;
	std::vector <Unit*>* content;
	Static* object;
	ObjectType type;
	double centerX;
	double centerY;
};

