#pragma once

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <vector>
#include "Entity.h"
#include "ObjectEnums.h"

using namespace Urho3D;

class Controls {
public:
	Controls(UI* _ui, Graphics* _graphics);
	~Controls();
	bool raycast(float maxDistance, Vector3 & hitPos, Drawable *& hitDrawable, Camera * camera, Scene * scene);
	void select(Entity* entity);
private:
	UI* ui;
	Graphics* graphics;
	std::vector<Entity*> *selected;
	ObjectType selectedType;
};

