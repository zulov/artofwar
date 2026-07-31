#pragma once
#include <magic_enum.hpp>
#include <vector>
#include <Urho3D/Math/Color.h>

namespace Urho3D {
	class Vector3;
	class CustomGeometry;
}

enum class CellState : char;

enum class DebugLineType : char {
	UNIT_LINES,
	MAIN_GRID,
	INFLUENCE
};

class DebugLineRepo {
public:
	~DebugLineRepo();

	static void commit(DebugLineType type);
	static void beginGeometry(DebugLineType type);
	static void clear(DebugLineType type);
	static void drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                     const Urho3D::Color& color = Urho3D::Color::WHITE);
	static void drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                         const Urho3D::Vector3& third, const Urho3D::Color& color = Urho3D::Color::WHITE);

	static void drawQuad(DebugLineType type, const std::array<Urho3D::Vector3, 4>& corners,
						 const Urho3D::Color& color = Urho3D::Color::WHITE);
	static void init(DebugLineType type);
	static void dispose();

private:
	DebugLineRepo() = default;
	static std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> geometry;

};
