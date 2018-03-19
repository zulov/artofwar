#pragma once
#include "Unit.h"
#include <vector>


class UnitFactory
{
public:
	UnitFactory();
	~UnitFactory();

	std::vector<Unit*>* create(unsigned int number, int id, Vector2& center, int player, int level);
	std::vector<Unit*>* load(dbload_unit* unit);
private:
	std::vector<Unit*>* units;
};
