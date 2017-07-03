#pragma once
#include "Unit.h"
#include <vector>
#include "EntityFactory.h"
#include "defines.h"
#include "SpacingType.h"
#include "UnitType.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Model.h>
#include "LinkComponent.h"
#include "Game.h"


class UnitFactory :public EntityFactory {
public:
	UnitFactory();
	~UnitFactory();

	std::vector<Unit*>* create(unsigned int number, UnitType unitType, Vector3 *center, SpacingType spacing, int player);

};

 