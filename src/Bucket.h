#pragma once
#include <vector>
#include "Entity.h"
#include <algorithm>
class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Entity*> getContent();
	void add(Entity* entity);
	void remove(Entity* entity);
	void clearContent();
	int getSum();
	void addToSum();
	void clearSum();

private:
	int sum;
	std::vector <Entity*> content;
	Vector3 position;
};

