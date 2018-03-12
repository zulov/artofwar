#pragma once
#include "objects/LinkComponent.h"
#include <Urho3D/Graphics/Drawable.h>


struct hit_data
{
	Urho3D::Vector3 position;
	Urho3D::Drawable* drawable;
	LinkComponent* link;
};
