#pragma once
#include <Urho3D/Resource/ResourceCache.h>
#include "SpacingType.h"


using namespace Urho3D;

class EntityFactory {
public:
	EntityFactory();
	~EntityFactory();
protected:
	double getSpecSize(SpacingType spacing);
};

