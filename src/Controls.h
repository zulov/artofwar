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

class Controls
{
public:
	Controls();
	~Controls();
	bool raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera);
	void unselect(Entity* entity);
	void select(Entity* entity);
	void unSelect(int type);
	void action(ActionType action, Entity* entity);

	void clickDownRight(Vector3 hitPos);
	void clickDownLeft(Vector3 hitPos);
	void clickDown(const int button);

	void clickLeft(Drawable* hitDrawable, Vector3 hitPos);
	void click(int button);
	void clickRight(Drawable* hitDrawable, Vector3 hitPos);
	void leftReleased(std::pair<Entity*, Entity*>* held);
	void release(const int button);
private:
	std::vector<Entity*>* selected;
	ObjectType selectedType;
	float maxDistance = 300;

	bool mouseLeftHeld = false;
	bool mouseRightHeld = false;
	bool mouseMiddleHeld = false;

	std::pair<Entity*, Entity*> *leftHeld;
	std::pair<Entity*, Entity*> *middleHeld;
	std::pair<Entity*, Entity*> *rightHeld;

};
