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

	std::vector<Unit*>& create(unsigned int number, unsigned short id, const Urho3D::Vector2& center, unsigned char playerId);
	std::vector<Unit*>& load(dbload_unit* unit);
private:
	std::vector<Unit*> units;
};
