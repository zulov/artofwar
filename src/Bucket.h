#pragma once
#include <vector>
#include "Unit.h"
#include <list>

class Bucket {
public:
	Bucket();
	~Bucket();

	list<Entity*> *getContent();
	void add(Entity* entity);
	void remove(Entity* entity);
	void clearContent();
private:
	list<Entity*>* content;
};

