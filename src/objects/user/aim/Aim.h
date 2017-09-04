#pragma once
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

class Aim
{
public:
	Aim(Urho3D::Vector3* _position);
	~Aim();
	Urho3D::Vector3* getPosition();
	double getRadius();
private:
	Urho3D::Vector3* position;
	double radius;
	Urho3D::Node* node;
};
