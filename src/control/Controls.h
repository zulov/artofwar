#pragma once


#include <Urho3D/Graphics/Renderer.h>
#include <vector>
#include "ControlsState.h"
#include "ObjectEnums.h"
#include "OrderType.h"
#include "SelectedInfo.h"
#include "commands/creation/CreationCommand.h"
#include "hud/HudElement.h"
#include <Urho3D/Input/Input.h>
#include "MouseButton.h"
#include <Urho3D/Graphics/StaticModel.h>


struct MouseButton;
class SimulationInfo;
struct hit_data;
using namespace Urho3D;

class Controls
{
public:
	void createNode(String model, String texture, Urho3D::Node** node);
	Controls(Input* _input);
	~Controls();
	
	void select(Physical* entity);
	void unSelectAll();

	void hudAction(HudElement* hud);

	bool clickDown(MouseButton& var, hit_data hitData);
	SelectedInfo* getInfo();
	ControlsState getState();
	void cleanMouse();

	void deactivate();
	void activate();

	void order(short id);
	void clean(SimulationInfo* simulationInfo);
	void updateSelection();
	void updateArrow();

	void defaultControl();
	void buildControl();

	void orderControl();
	void control();

private:
	void orderUnit(short id);
	void orderBuilding(short id);

	void refreshSelected();
	bool raycast(hit_data& hitData, Camera* camera);

	void startArrowNode(const hit_data& hitData);
	void startSelectionNode(hit_data hitData);
	
	void clickDownLeft();
	void clickDownRight();
	void createBuilding(Vector3& pos);

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

	Urho3D::Node* selectionNode;	
	Urho3D::Node* arrowNode;

	short idToCreate = -1;
	double clickDistance = 2 * 2;
	bool active = true;

};
