#pragma once
#include <vector>
#include "objects/unit/Unit.h"

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

private:
	std::vector <Unit*>* content;
	Static* object;
	ObjectType type;
};

