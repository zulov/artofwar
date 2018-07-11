#pragma once
#include "ColorPallet.h"
#include "objects/unit/state/UnitState.h"
#include <Urho3D/Graphics/Material.h>


#define PALLET_RESOLUTION 32


class ColorPeletteRepo
{
public:
	ColorPeletteRepo();
	~ColorPeletteRepo();
	Urho3D::Material* getColor(ColorPallet colorPallet, float value, float maxValue);
	Urho3D::Material* getColor(UnitState state);
	Urho3D::Material* getLineMaterial() const;
private:
	Urho3D::Material* redPallet[PALLET_RESOLUTION + 1];
	Urho3D::Material* statePallet[STATE_SIZE];
	Urho3D::Material* lineMaterial;
};
