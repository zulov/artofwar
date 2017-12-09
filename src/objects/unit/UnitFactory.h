#pragma once
#include "Unit.h"
#include <vector>
#include "objects/EntityFactory.h"


class UnitFactory : public EntityFactory
{
public:
	UnitFactory();
	~UnitFactory();

	std::vector<Unit*>* create(unsigned int number, int id, Vector3* center, int player);
	std::vector<Unit*>* load(dbload_unit* unit);
private:
	std::vector<Unit*>* units;
	StateManager* states;
};
