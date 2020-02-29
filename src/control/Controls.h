#pragma once
#include "ControlsState.h"
#include "MouseButton.h"
#include "SelectedInfo.h"
#include "objects/order/enums/UnitAction.h"
#include "commands/creation/CreationCommand.h"
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Input/Input.h>
#include <vector>

#define MAX_DEPLOY_MARK_NUMBER 5

enum class BuildingActionType : char;
enum class ResourceActionType : char;
class HudData;
class SimulationInfo;
struct ActionParameter;
struct MouseButton;
struct hit_data;

class Controls {
public:
	explicit Controls(Urho3D::Input* _input);
	~Controls();

	void updateAdditionalInfo() const;
	void select(std::vector<Physical*>* entities) const;
	void selectOne(Physical* entity, char player) const;
	void unSelectAll() const;

	void toBuild(HudData* hud);

	void order(short id, const ActionParameter& parameter);

	void cleanMouse();

	void deactivate();
	void activate();
	void unitOrder(short id);

	void clean(SimulationInfo* simulationInfo) const;

	void updateSelection() const;
	void updateArrow() const;

	void defaultControl();
	void buildControl();
	void orderControl();

	void control();

	SelectedInfo* getInfo() const { return selectedInfo; }
private:
	void executeOnUnits(short id) const;
	void executeOnResources(ResourceActionType action) const;
	void executeOnBuildings(BuildingActionType action, short id) const;

	void unitFormation(short id) const;
	void actionUnit(short id, const ActionParameter& parameter);

	void refreshSelected() const;
	bool conditionToClean(SimulationInfo* simulationInfo) const;

	bool clickDown(MouseButton& var) const;
	void createBuilding(Urho3D::Vector2 pos) const;

	void leftClick(hit_data& hitData) const;
	void leftDoubleClick(hit_data& hitData) const;
	void leftClickBuild(hit_data& hitData) const;
	void rightClick(hit_data& hitData) const;

	void leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const;
	void rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const;

	void releaseRight();
	bool orderAction() const;
	void releaseLeft();
	void releaseBuildLeft();
	void toDefault();

	std::vector<Physical*>* selected; //TODO to powinien byæ set

	Urho3D::Input* input;

	Urho3D::Node* deployMark[MAX_DEPLOY_MARK_NUMBER];
	Urho3D::Node* selectionNode{};
	Urho3D::Node* arrowNode{};
	Urho3D::Node* tempBuildingNode{};
	Urho3D::StaticModel* tempBuildingModel;

	SelectedInfo* selectedInfo;

	float clickDistance = 2 * 2;
	short idToCreate = -1;
	bool active = true;

	ControlsState state = ControlsState::DEFAULT;
	UnitAction unitOrderType = UnitAction::GO;
	ObjectType typeToCreate;

	MouseButton left;
	MouseButton right;
};
