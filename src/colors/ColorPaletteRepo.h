#pragma once
#include <magic_enum.hpp>
#include <Urho3D/Math/Color.h>
#include "objects/unit/state/UnitState.h"

enum class CellState : char;
enum class ColorPallet : char;
namespace Urho3D { class Material; }


constexpr char PALLET_RESOLUTION = 32;
constexpr char SPECTRUM_RESOLUTION = 64;

class ColorPaletteRepo {
public:
	ColorPaletteRepo();
	~ColorPaletteRepo() = default;
	Urho3D::Material* getColor(ColorPallet colorPallet, float value, float maxValue);
	Urho3D::Color& getColor(float value, float maxValue);
	Urho3D::Material* getColor(UnitState state);
	Urho3D::Material* getLineMaterial() const;
	std::tuple<bool, Urho3D::Color> getInfoForGrid(CellState state);
private:

	Urho3D::Material* redPallet[PALLET_RESOLUTION + 1];
	Urho3D::Material* statePallet[magic_enum::enum_count<UnitState>()];
	Urho3D::Material* lineMaterial;
	Urho3D::Color basicSpectrum[SPECTRUM_RESOLUTION + 1];
};
