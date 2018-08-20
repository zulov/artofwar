#pragma once

#include "ControlsState.h"
#include "MouseButton.h"
#include "SelectedInfo.h"
#include "UnitOrder.h"
#include "commands/creation/CreationCommand.h"
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Input/Input.h>
#include <vector>


struct ActionParameter;
class HudData;
struct MouseButton;
class SimulationInfo;
struct hit_data;

class Controls
{
public:
	explicit Controls(Urho3D::Input* _input);
	~Controls();

	void select(Physical* entity);
	void unSelectAll();

	void hudAction(HudData* hud);

	bool clickDown(MouseButton& var, hit_data& hitData);
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

	void showNode(Urho3D::Node* node, const hit_data& hitData) const;
	void unitFormation(short id);
	void actionUnit(short id, ActionParameter& parameter);
	void orderBuilding(short id, ActionParameter& parameter);

	void refreshSelected();
	bool conditionToClean(SimulationInfo* simulationInfo);

	void clickDown(MouseButton& var, Urho3D::Node* node);
	void createBuilding(Urho3D::Vector2 pos);

	void leftClick(hit_data &hitData);
	void leftClickBuild(hit_data &hitData);
	void rightClickDefault(hit_data &hitData, bool shiftPressed);

	void leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held);
	void rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held);

	void releaseRight();
	bool orderAction(bool shiftPressed);
	void releaseLeft();
	void releaseBuildLeft();
	void resetState();
	void toDefault();

	std::vector<Physical*>* selected; //TODO to powinien byæ set

	MouseButton left;
	MouseButton right;

	Urho3D::Input* input;

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
