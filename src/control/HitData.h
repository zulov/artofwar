#pragma once
#include <Urho3D/Graphics/Drawable.h>
#include "objects/LinkComponent.h"


struct hit_data
{
	Urho3D::Vector3 position;
	Urho3D::Drawable* drawable;
	LinkComponent* link;
};
