#pragma once
#include <Urho3D/Graphics/CustomGeometry.h>

class DebugLineRepo
{
public:
	~DebugLineRepo();
	static void commit();
	static void beginGeometry();
	static void clear();
	static void drawLine(const Urho3D::Vector3& first, const Urho3D::Vector3& second, const Urho3D::Color& color = Urho3D::Color::WHITE);

	static Urho3D::CustomGeometry* geometry;
	static void init();
private:
	DebugLineRepo();
};
