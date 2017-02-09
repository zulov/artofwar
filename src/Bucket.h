#pragma once
#include <vector>
#include "Unit.h"
#include <algorithm>
class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Unit*> *getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	void clearContent();
	int getSum();
	void addToSum();
	void clearSum();

private:
	int sum;
	std::vector <Unit*>* content;
	Vector3 position;
};

