#pragma once
#include <vector>
#include "Aims.h"

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

