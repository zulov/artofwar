#pragma once
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Resource/Image.h>

namespace Urho3D {
	class String;
}

class MapReader
{
public:
	MapReader();
	~MapReader();
	float getValue(Urho3D::Image* image, int i, int j);
	void append(Urho3D::Image* image, float* vertexData, int& index, int i, int j);
	Urho3D::Model* read(Urho3D::String path);
};

