#pragma once
#include "Aims.h"
#include <vector>


class AimContainer
{
public:
	AimContainer();
	~AimContainer();
	void clean();
	Aims* getNext();
	Aims* getCurrent();
private:
	std::vector<Aims*> aims;
	int index;
};

