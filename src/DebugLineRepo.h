#pragma once
#include <Urho3D/Graphics/CustomGeometry.h>

class DebugLineRepo
{
public:
	~DebugLineRepo();
	static void commit();
	static void beginGeometry();
	static void clear();

	static Urho3D::CustomGeometry* geometry;
	static void init();
private:
	DebugLineRepo();
};
