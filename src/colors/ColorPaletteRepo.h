#pragma once
#include "ColorPallet.h"
#include "objects/unit/state/UnitState.h"
#include <Urho3D/Graphics/Material.h>
#include <magic_enum.hpp>


#define PALLET_RESOLUTION 32
#define SPECTRUM_RESOLUTION 64

class ColorPaletteRepo {
public:
	ColorPaletteRepo();
	~ColorPaletteRepo();
	Urho3D::Material* getColor(ColorPallet colorPallet, float value, float maxValue);	
	Urho3D::Color& getColor(float value, float maxValue);
	Urho3D::Material* getColor(UnitState state);
	Urho3D::Material* getLineMaterial() const;
private:
	float fixValue(float value, float maxValue);
	Urho3D::Material* redPallet[PALLET_RESOLUTION + 1];
	Urho3D::Material* statePallet[magic_enum::enum_count<UnitState>()];
	Urho3D::Material* lineMaterial;
	Urho3D::Color basicSpectrum[SPECTRUM_RESOLUTION+1];
};
