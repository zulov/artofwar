#pragma once
#include <vector>
#include "ControlsState.h"
#include "MouseButton.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "BillboardSetProvider.h"
#include "database/db_strcut.h"

enum class ActionType : char;
class SimInfo;
constexpr char MAX_DEPLOY_MARK_NUMBER = 5;

namespace Urho3D {
	class StaticModel;
	class Input;
	class Vector3;
	class Vector2;
	class Node;
}

enum class BuildingActionType : char;
enum class ResourceActionType : char;
class HudData;
class ObjectsInfo;
class Physical;
class SelectedInfo;
class UnitOrder;
struct ActionParameter;
struct hit_data;

class Controls {
public:
	explicit Controls(Urho3D::Input* _input);
	~Controls();

	void init();

	void updateAdditionalInfo() const;
	void setCircle(int i, Urho3D::Vector4 val) const;
	void resetCircles() const;
	void select(const std::vector<Physical*>* entities);
	void selectOne(Physical* entity, char player);
	void unSelectAll();

	void toBuild(HudData* hud);

	void order(short id, ActionType type);

	void cleanMouse();

	void deactivate();
	void activate();
	void unitOrder(short id);

	void cleanAndUpdate(const SimInfo* simulationInfo);

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
	void actionUnit(short id, ActionType type);

	void refreshSelected();
	bool conditionToClean(const SimInfo* simulationInfo) const;
	void setCircleSight(int i, const Urho3D::Vector3& position, float radius, Urho3D::Color color) const;

	bool clickDown(MouseButton& var) const;
	void createBuilding(Urho3D::Vector2 pos) const;

	void leftClick(hit_data& hitData);
	void leftDoubleClick(hit_data& hitData);
	void leftClickBuild(hit_data& hitData);

	BuildingActionType getBuildingActionType(ActionType type);

	UnitOrder* vectorOrder(UnitAction order, Urho3D::Vector2* vector, bool shiftPressed,
	                       const std::vector<Physical*>& vec) const;
	UnitOrder* thingOrder(UnitAction order, Physical* toUse, bool shiftPressed,
	                      const std::vector<Physical*>& vec) const;
	UnitAction chooseAction(hit_data& hitData) const;
	UnitOrder* createUnitOrder(hit_data& hitData) const;

	void rightClick(hit_data& hitData) const;

	void leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held);
	void rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const;

	void releaseRight();
	bool orderAction() const;
	void releaseLeft();
	void releaseBuildLeft();
	void toDefault();

	std::vector<Physical*> selected;

	Urho3D::Input* input;

	Urho3D::Node* deployMark[MAX_DEPLOY_MARK_NUMBER];
	Urho3D::Node* arrowNode{};
	Urho3D::Node* tempBuildingNode{};

	SelectedInfo* selectedInfo{};

	BillboardSetProvider billboardSetProvider;

	float clickDistance = 2 * 2;
	short idToCreate = -1;
	bool active = true;

	ControlsState state = ControlsState::DEFAULT;
	UnitAction unitOrderType = UnitAction::GO;
	ObjectType typeToCreate;

	MouseButton left;
	MouseButton right;
};
