#pragma once
#include "ColorPallet.h"
#include <Urho3D/Graphics/Material.h>

#define PALLET_RESOLUTION 32


class ColorPeletteRepo
{
public:
	ColorPeletteRepo();
	~ColorPeletteRepo();
	Urho3D::Material* getColor(ColorPallet colorPallet, float value, float maxValue);
private:
	Urho3D::Material* redPallet[PALLET_RESOLUTION + 1];
};
