#pragma once
#include <Urho3D/Graphics/CustomGeometry.h>

#define NUMBER_OF_GEOMETRIES 3

enum class CellState : char;
enum class DebugLineType : char;

class DebugLineRepo {


public:
	~DebugLineRepo();
	static std::tuple<bool, Urho3D::Color> getInfoForGrid(CellState state);
	static void commit(DebugLineType type, short batch =0);
	static void beginGeometry(DebugLineType type, short batch =0);
	static void clear(DebugLineType type, short batch =0);
	static void drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                     const Urho3D::Color& color = Urho3D::Color::WHITE, short batch =0);
	static void drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	                         const Urho3D::Vector3& third, const Urho3D::Color& color = Urho3D::Color::WHITE, short batch =0);
	static void init(DebugLineType type);
	static void init(DebugLineType type, short batches);
private:
	DebugLineRepo();
	static std::vector<Urho3D::CustomGeometry*> geometry[NUMBER_OF_GEOMETRIES];

};

enum class DebugLineType : char {
	UNIT_LINES,
	MAIN_GRID,
	INFLUANCE
};
