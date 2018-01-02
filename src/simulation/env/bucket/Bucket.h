#pragma once
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include <vector>



class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Unit*>& getContent();
	void add(Unit* entity);
	void remove(Unit* entity);

	int& getSize();
	bool incUnitsPerPlayer(content_info* ci, int activePlayer, const bool checks[]);
private:
	int size;
	int unitsNumberPerPlayer[MAX_PLAYERS];
	std::vector<Unit*> content;
};
