#pragma once
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Math/Color.h>

struct db_building;
enum class VisibilityType : char;
enum class CellState : char;
enum class ColorPallet : char;

namespace Urho3D {
	class Material;
}


constexpr char PALLET_RESOLUTION = 32;
constexpr char SPECTRUM_RESOLUTION = 64;

class ColorPaletteRepo {
public:
	ColorPaletteRepo();
	~ColorPaletteRepo() = default;
	Urho3D::Color& getColor(float value, float maxValue);
	Urho3D::Color& getSolidColor(float value, float maxValue);
	Urho3D::Material* getLineMaterial() const;
	Urho3D::Material* getInfluenceMaterial() const;
	std::tuple<bool, Urho3D::Color> getInfoForGrid(CellState state) const;
	Urho3D::Color getCircleColor(db_building* dbBuilding) const;
	Urho3D::Color getColorForValidation(const db_building* building, Urho3D::Vector2& hitPos) const;
private:
	Urho3D::Material* lineMaterial;
	Urho3D::Material* influenceMaterial;
	Urho3D::Color basicSpectrum[SPECTRUM_RESOLUTION + 1];
	Urho3D::Color basicSpectrumSolid[SPECTRUM_RESOLUTION + 1];//solid
};
