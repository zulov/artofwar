#pragma once
#include <magic_enum.hpp>
#include <vector>
#include <Urho3D/Math/Color.h>

namespace Urho3D {
	class Vector3;
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

	static void drawQuads(DebugLineType type, const std::vector<const std::array<Urho3D::Vector3, 4>>& points,
	                      const Urho3D::Color& color);

	static void drawQuads(DebugLineType type, unsigned short resolution, unsigned char*, float maxValue);
	static void drawQuads(DebugLineType type, unsigned short resolution, float*, float maxValue);

	static void dispose();

private:
	DebugLineRepo() = default;
	static std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> geometries;
	static std::unordered_map<unsigned short, std::vector<std::array<Urho3D::Vector3, 4>>> quadCords;

};
