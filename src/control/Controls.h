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


class HudData;
class SimulationInfo;
struct ActionParameter;
struct MouseButton;
struct hit_data;

class Controls
{
public:
	explicit Controls(Urho3D::Input* _input);
	~Controls();

	void select(std::vector<Physical*>* entities);
	void select(Physical* physical);
	void unSelectAll();

	void toBuild(HudData* hud);

	void order(short id, const ActionParameter& parameter);

	bool clickDown(MouseButton& var, hit_data& hitData);
	void cleanMouse();

	void deactivate();
	void activate();
	void unitOrder(short id);

	void clean(SimulationInfo* simulationInfo);

	void updateSelection();
	void updateArrow();

	void defaultControl();
	void buildControl();
	void orderControl();
	void resourceControl();

	void control();

	SelectedInfo* getInfo() const { return selectedInfo; }
private:
	void selectOne(Physical* entity);

	void orderPhysical(short id, const ActionParameter& parameter) const;
	void orderResource(short id, const ActionParameter& parameter);

	void executeOnAll(short id, const ActionParameter& parameter);

	void unitFormation(short id);
	void actionUnit(short id, const ActionParameter& parameter);
	void orderBuilding(short id, const ActionParameter& parameter);

	void refreshSelected();
	bool conditionToClean(SimulationInfo* simulationInfo);

	void clickDown(MouseButton& var, Urho3D::Node* node);
	void createBuilding(Urho3D::Vector2 pos);

	void leftClick(hit_data& hitData);
	void leftClickBuild(hit_data& hitData);
	void rightClickDefault(hit_data& hitData, bool shiftPressed);

	void leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held);
	void rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held);

	void releaseRight();
	bool orderAction(bool shiftPressed);
	void releaseLeft();
	void releaseBuildLeft();
	void resetState();
	void toDefault();
	void toResource();

	std::vector<Physical*>* selected; //TODO to powinien byæ set

	MouseButton left;
	MouseButton right;

	Urho3D::Input* input;

	ControlsState state = ControlsState::DEFAULT;
	UnitOrder unitOrderType = UnitOrder::GO;
	ObjectType typeToCreate;
	SelectedInfo* selectedInfo;

	Urho3D::Node* selectionNode{};
	Urho3D::Node* arrowNode{};
	Urho3D::Node* tempBuildingNode{};
	Urho3D::StaticModel* tempBuildingModel;

	short idToCreate = -1;
	float clickDistance = 2 * 2;
	bool active = true;
};
