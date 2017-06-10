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
#include <Urho3D/Input/Input.h>
#include "ControlsState.h"
#include "Game.h"
#include "SimulationCommand.h"
#include "BuildCommand.h"
#include "LinkComponent.h"
#include "Mediator.h"
#include "HudElement.h"
#include "SelectedInfo.h"


using namespace Urho3D;

class Controls
{
public:
	Controls(Input* _input);
	~Controls();
	bool raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera);
	void unselect(Entity* entity);
	void select(Entity* entity);
	void controlEntity(Vector3 hitPos, bool ctrlPressed, Entity* clicked);
	void unSelect(int type);
	void action(ActionType action, Entity* entity);

	void clickDownRight(Vector3 hitPos);
	void clickDownLeft(Vector3 hitPos);
	void clickDown(const int button);

	void build(Vector3* pos);
	void deploy(Vector3* pos);
	void leftClick(Drawable* hitDrawable, Vector3 hitPos);
	void leftHold(std::pair<Entity*, Entity*>* held);

	void rightClick(Drawable* hitDrawable, Vector3 hitPos);
	void rightHold(std::pair<Entity*, Entity*>* pair);
	void release(const int button);
	void updateState(ControlsState state);
	void hudAction(HudElement* hud);
	SelectedInfo* getSelectedInfo();
private:
	std::vector<Entity*>* selected;//TODO to powinien byæ set
	ObjectType selectedType;
	float maxDistance = 300;

	bool mouseLeftHeld = false;
	bool mouseRightHeld = false;
	bool mouseMiddleHeld = false;

	std::pair<Entity*, Entity*>* leftHeld;
	std::pair<Entity*, Entity*>* middleHeld;
	std::pair<Entity*, Entity*>* rightHeld;

	Input* input;
	double clickDistance = 2;
	ControlsState state = ControlsState::SELECT;
	BuildingType toBuild;
	UnitType toDeploy;
	Entity* temp;
	SelectedInfo* selectedInfo;
	int selectedEntititesTypes[SELECTED_INFO_SIZE];
};
