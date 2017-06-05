#pragma once
#include <vector>
#include "Unit.h"

class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Entity*> *getContent();
	void add(Entity* entity);
	void remove(Entity* entity);
	void clearContent();
private:
	std::vector <Entity*>* content;
};

