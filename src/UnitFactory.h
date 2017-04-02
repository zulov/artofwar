#pragma once
#include "Unit.h"
#include <vector>
#include "EntityFactory.h"
#include "defines.h"

class UnitFactory :public EntityFactory {
public:
	UnitFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene);
	~UnitFactory();
	void createLink(Node* node, Unit* newUnit);
	std::vector<Unit*>* createUnits();

	int size = UNITS_NUMBER;
	double space = 1;
};

 