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
#include "objects/Entity.h"
#include "ObjectEnums.h"
#include <Urho3D/Input/Input.h>
#include "ControlsState.h"
#include "Game.h"
#include "commands/SimulationCommand.h"
#include "commands/BuildCommand.h"
#include "objects/LinkComponent.h"
#include "Mediator.h"
#include "hud/HudElement.h"
#include "SelectedInfo.h"


using namespace Urho3D;

class Controls
{
public:
	Controls(Input* _input);
	~Controls();
	bool raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera);
	void select(Physical* entity);
	void controlEntity(Vector3 &hitPos, bool ctrlPressed, Physical* clicked);
	void unSelectAll();
	
	void clickDownRight(Vector3 &hitPos);
	void clickDownLeft(Vector3 &hitPos);
	void clickDown(const int button);
	void create(ObjectType type, Vector3* pos, int number);

	void leftClick(Drawable* hitDrawable, Vector3 &hitPos);
	void leftHold(std::pair<Vector3*, Vector3*>* held);

	void rightClick(Drawable* hitDrawable, Vector3 &hitPos);
	void rightHold(std::pair<Vector3*, Vector3*>* pair);
	void release(const int button);
	void updateState(SelectedInfo* selectedInfo);
	void hudAction(HudElement* hud);
	SelectedInfo* getInfo();
	void deactivate();
	bool isActive();
	void activate();
	void action(HudElement* hudElement);
	void refreshSelected();
	void clean(SimulationInfo *simulationInfo);

private:
	std::vector<Physical*>* selected;//TODO to powinien byæ set

	ObjectType selectedType;
	float maxDistance = 300;

	bool mouseLeftHeld = false;
	bool mouseRightHeld = false;
	bool mouseMiddleHeld = false;

	std::pair<Vector3*, Vector3*>* leftHeld;
	std::pair<Vector3*, Vector3*>* middleHeld;
	std::pair<Vector3*, Vector3*>* rightHeld;

	Input* input;
	double clickDistance = 2 * 2;
	ControlsState state = ControlsState::DEFAULT;

	short idToCreate=-1;
	ObjectType typeToCreate;

	SelectedInfo* selectedInfo;
	bool active = true;
};
