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
#include "commands/CreationCommand.h"
#include "commands/BuildCommand.h"
#include "objects/LinkComponent.h"
#include "Mediator.h"
#include "hud/HudElement.h"
#include "SelectedInfo.h"
#include "OrderType.h"
#include "MouseButton.h"


using namespace Urho3D;

class Controls
{
public:
	Controls(Input* _input);
	~Controls();
	
	void select(Physical* entity);
	void unSelectAll();

	void hudAction(HudElement* hud);
	SelectedInfo* getInfo();

	void deactivate();
	void activate();

	void order(short id);
	void clean(SimulationInfo* simulationInfo);
	void defaultControl();
	void buildControl();

	void orderControl();
	void control();

private:
	void orderUnit(short id);
	void orderBuilding(short id);

	void createBuilding(Vector3* pos);
	void refreshSelected();
	bool raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera);

	void clickDown(MouseButton &var);

	void leftClick(Physical* clicked, Vector3& hitPos);
	void leftClickBuild(Physical* clicked, Vector3& hitPos);
	void rightClickDefault(Physical* clicked, Vector3& hitPos, bool shiftPRessed);

	void leftHold(std::pair<Vector3*, Vector3*>* held);
	void rightHold(std::pair<Vector3*, Vector3*>* held);

	void releaseRight();
	bool orderAction(bool shiftPressed);
	void releaseLeft();
	void releaseBuildLeft();
	void resetState();	
	void toDefault();

	std::vector<Physical*>* selected;//TODO to powinien byæ set

	float maxDistance = 300;

	MouseButton left;
	MouseButton right;

	Input* input;
	ObjectType selectedType;
	ControlsState state = DEFAULT;
	OrderType orderType = OrderType::GO;
	ObjectType typeToCreate;
	SelectedInfo* selectedInfo;
	short idToCreate = -1;
	double clickDistance = 2 * 2;
	bool active = true;

};
