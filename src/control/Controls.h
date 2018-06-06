#pragma once

#include "ControlsState.h"
#include "MouseButton.h"
#include "UnitOrder.h"
#include "SelectedInfo.h"
#include "commands/creation/CreationCommand.h"
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <vector>
#include <Urho3D/Graphics/StaticModel.h>


struct ActionParameter;
class HudData;
struct MouseButton;
class SimulationInfo;
struct hit_data;
using namespace Urho3D;

class Controls
{
public:
	Controls(Input* _input);
	~Controls();

	void select(Physical* entity);
	void unSelectAll();

	void hudAction(HudData* hud);

	bool clickDown(MouseButton& var, hit_data hitData);
	void cleanMouse();
	SelectedInfo* getInfo();
	ControlsState getState();

	void deactivate();
	void activate();
	void unitOrder(short id);

	void orderPhysical(short id, ActionParameter& parameter);
	void order(short id, ActionParameter& parameter);
	void executeOnAll(short id, ActionParameter& parameter);
	void clean(SimulationInfo* simulationInfo);
	void updateSelection();
	void updateArrow();

	void defaultControl();
	void buildControl();

	void orderControl();
	void control();

private:
	void unitFormation(short id);
	void actionUnit(short id, ActionParameter& parameter);
	void orderBuilding(short id, ActionParameter& parameter);

	void refreshSelected();

	void startArrowNode(const hit_data& hitData);
	void startSelectionNode(hit_data hitData);

	void clickDownLeft();
	void clickDownRight();
	void createBuilding(Vector2& pos);

	void leftClick(Physical* clicked, Vector3& hitPos);
	void leftClickBuild(Physical* clicked, Vector3& hitPos);
	void rightClickDefault(Physical* clicked, Vector3& hitPos, bool shiftPressed);

	void leftHold(std::pair<Vector3*, Vector3*>& held);
	void rightHold(std::pair<Vector3*, Vector3*>& held);

	void releaseRight();
	bool orderAction(bool shiftPressed);
	void releaseLeft();
	void releaseBuildLeft();
	void resetState();
	void toDefault();

	std::vector<Physical*>* selected; //TODO to powinien byæ set

	MouseButton left;
	MouseButton right;

	Input* input;
	ObjectType selectedType;
	ControlsState state = DEFAULT;
	UnitOrder unitOrderType = UnitOrder::GO;
	ObjectType typeToCreate;
	SelectedInfo* selectedInfo;

	Urho3D::Node* selectionNode{};
	Urho3D::Node* arrowNode{};
	Urho3D::Node* tempBuildingNode{};
	Urho3D::StaticModel *tempBuildingModel;

	short idToCreate = -1;
	float clickDistance = 2 * 2;
	bool active = true;
};
