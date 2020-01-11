#pragma once

#include <vector>

struct dbload_unit;

namespace Urho3D {
	class Vector2;
}

class Unit;

class UnitFactory
{
public:
	UnitFactory();
	~UnitFactory();

	std::vector<Unit*>* create(unsigned int number, int id, Urho3D::Vector2& center, int player, int level) const;
	std::vector<Unit*>* load(dbload_unit* unit) const;
private:
	std::vector<Unit*>* units;
};
