#pragma once
#include <Urho3D/Graphics/Model.h>

namespace Urho3D {
	class String;
}

class MapReader
{
public:
	MapReader();
	~MapReader();
	Urho3D::Model* read(Urho3D::String path);
};

