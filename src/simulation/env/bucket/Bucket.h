#pragma once
#include "defines.h"
#include <vector>


struct content_info;
class Unit;

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
