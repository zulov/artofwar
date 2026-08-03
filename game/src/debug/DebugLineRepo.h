#pragma once
#include <array>
#include <magic_enum.hpp>
#include <unordered_map>
#include <vector>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Vector3.h>

namespace Urho3D {
	class CustomGeometry;
}

enum class DebugLineType : char {
	UNIT_LINES,
	GRID
};

class DebugLineRepo {
public:
	~DebugLineRepo();

	static void init(DebugLineType type);
	static void commit(DebugLineType type);
	static void beginGeometry(DebugLineType type);
	static void clear(DebugLineType type);
	static void drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                     const Urho3D::Color& color = Urho3D::Color::WHITE);

	static void drawQuads(DebugLineType type, unsigned short resolution, const unsigned char* values,
	                     float maxValue);
	static void drawQuads(DebugLineType type, unsigned short resolution, const float* values, float maxValue);
	static void drawQuads(DebugLineType type, unsigned short resolution, const std::vector<Urho3D::Color>& colors);
	static void drawQuad(DebugLineType type, unsigned short resolution, int index, const Urho3D::Color& color);

	static void dispose();

private:
	DebugLineRepo() = default;
	static void defineQuad(Urho3D::CustomGeometry* geom, const std::array<Urho3D::Vector3, 4>& corners,
	                       const Urho3D::Color& color);
	static std::vector<std::array<Urho3D::Vector3, 4>>& getQuadCords(unsigned short resolution);
	static std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> geometries;
	static std::unordered_map<unsigned short, std::vector<std::array<Urho3D::Vector3, 4>>> quadCords;

};
