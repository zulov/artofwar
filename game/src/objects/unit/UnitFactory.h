#pragma once
#include <vector>


namespace Urho3D {
	class Vector2;
}

struct dbload_unit;
class Unit;

class UnitFactory {
public:
	UnitFactory();
	~UnitFactory();

	std::vector<Unit*>& create(unsigned int number, short id, Urho3D::Vector2& center, short playerId, short level);
	std::vector<Unit*>& load(dbload_unit* unit);
private:
	std::vector<Unit*> units;
};
