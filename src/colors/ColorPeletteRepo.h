#pragma once
#include "ColorPallet.h"
#include "objects/unit/state/UnitStateType.h"
#include <Urho3D/Graphics/Material.h>


#define PALLET_RESOLUTION 32


class ColorPeletteRepo
{
public:
	ColorPeletteRepo();
	~ColorPeletteRepo();
	Urho3D::Material* getColor(ColorPallet colorPallet, float value, float maxValue);
	Urho3D::Material* getColor(UnitStateType state);
private:
	Urho3D::Material* redPallet[PALLET_RESOLUTION + 1];
	Urho3D::Material* statePallet[STATE_SIZE];
};
