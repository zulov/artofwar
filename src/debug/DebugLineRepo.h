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

	static void commit(DebugLineType type, short batch = 0);
	static void beginGeometry(DebugLineType type, short batch = 0);
	static void clear(DebugLineType type, short batch = 0);
	static void drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                     const Urho3D::Color& color = Urho3D::Color::WHITE, short batch = 0);
	static void drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                         const Urho3D::Vector3& third, const Urho3D::Color& color = Urho3D::Color::WHITE, short batch =0);
	static void init(DebugLineType type);
	static void init(DebugLineType type, short batches);
private:
	DebugLineRepo() = default;
	static std::vector<Urho3D::CustomGeometry*> geometry[magic_enum::enum_count<DebugLineType>()];

};
