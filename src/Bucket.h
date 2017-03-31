#pragma once
#include <vector>
#include "Unit.h"

class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Unit*> *getContent();
	void add(Unit* entity);
	void remove(Unit* entity);
	void clearContent();
private:
	std::vector <Unit*>* content;
	Vector3 corners[4];
};

