#pragma once
#include "objects/unit/Unit.h"
#include <vector>
#include "defines.h"


struct content_info;

class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Unit*>& getContent() { return content; }
	void add(Unit* entity);
	void remove(Unit* entity);

	bool incUnitsPerPlayer(content_info* ci, int activePlayer, const bool checks[]);
private:
	char unitsNumberPerPlayer[MAX_PLAYERS];
	std::vector<Unit*> content;
};
