#pragma once
#include "EntityFactory.h"
#include "Building.h"
#include "BuildingType.h"
#include "SpacingType.h"
#include <vector>
#include "Building.h"
#include "SpacingType.h"
#include <vector>
#include "Unit.h"
#include "Game.h"
#include "BuildingType.h"
#include <Urho3D/UI/Font.h>

class BuildingFactory :public EntityFactory {
public:
	BuildingFactory();
	~BuildingFactory();
	std::vector<Building*>* create(unsigned number, BuildingType unitType, Vector3* center, SpacingType spacingType);
};

