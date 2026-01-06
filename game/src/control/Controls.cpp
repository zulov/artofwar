#include "Controls.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include "ControlsUtils.h"
#include "SelectedInfo.h"
#include "colors/ColorPaletteRepo.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/action/ResourceActionCommand.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "hud/HudData.h"
#include "math/MathUtils.h"
#include "objects/NodeUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/UnitConst.h"
#include "player/PlayersManager.h"
#include "player/ai/ActionCenter.h"
#include "env/Environment.h"
#include "objects/ActionType.h"
#include "objects/PhysicalUtils.h"
#include "player/Player.h"
#include "simulation/FrameInfo.h"
#include "utils/OtherUtils.h"

constexpr char CIRCLE_NUMBER = 5;

Controls::Controls(Urho3D::Input* _input): input(_input), typeToCreate(ObjectType::NONE) {
	if (!SIM_GLOBALS.HEADLESS) {
		selectedInfo = new SelectedInfo();
		selected.reserve(5000);
		arrowNode = createNode("Objects/additional/arrowNode.xml");

		tempBuildingNode = Game::getScene()->CreateChild();
		tempBuildingNode->SetEnabled(false);

		selectedInfo->setSelectedType(ObjectType::NONE);

		for (int i = 0; i < MAX_DEPLOY_MARK_NUMBER; ++i) {
			deployMark[i] = createNode("Objects/buildings/additional/banner.xml");
		}
	}
}

Controls::~Controls() {
	delete selectedInfo;
	if (!SIM_GLOBALS.HEADLESS) {
		arrowNode->Remove();

		for (auto mark : deployMark) {
			mark->Remove();
		}
	}
}

void Controls::updateAdditionalInfo() const {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::BUILDING: {
		int min = Urho3D::Min(MAX_DEPLOY_MARK_NUMBER, selected.size());
		for (int i = 0; i < min; ++i) {
			deployMark[i]->SetEnabled(true);
			auto deployIndex = static_cast<Building*>(selected.at(i))->getDeploy().value();
			deployMark[i]->SetPosition(Game::getEnvironment()->getPosWithHeightAt(deployIndex));
		}
		for (int i = min; i < MAX_DEPLOY_MARK_NUMBER; ++i) {
			deployMark[i]->SetEnabled(false);
		}
	}
	break;
	default:
		for (const auto mark : deployMark) {
			mark->SetEnabled(false);
		}
	}
}

void Controls::setCircle(int i, Urho3D::Vector4 val) const {
	Game::getEnvironment()->setTerrainShaderParam("RangeData" + Urho3D::String(i), val);
}

void Controls::resetCircles() const {
	for (int i = 0; i < CIRCLE_NUMBER; ++i) {
		setCircle(i, Urho3D::Vector4());
	}
}

void Controls::setCircleSight(int i, const Urho3D::Vector3& position, float radius, Urho3D::Color color) const {
	color.a_ = 0;
	setCircle(i, Urho3D::Vector4(position.x_, position.z_, radius, float(color.ToUInt())));
}

void Controls::unSelectAll() {
	for (const auto& phy : selected) {
		phy->unSelect();
	}

	selected.clear();
	selectedInfo->setSelectedType(ObjectType::NONE);
	selectedInfo->reset();
	resetCircles();
}

void Controls::selectOne(Physical* entity, char player) {
	const auto entityType = entity->getType();
	if (!entity->isNodeEnabled()) {
		return;
	}
	if (entity == nullptr || entityType != selectedInfo->getSelectedType()) {
		unSelectAll();
	}
	if (!entity->isSelected() && entity->isAlive()
		&& (entity->getPlayer() < 0 || entity->getPlayer() == player)) {
		entity->select();

		selected.push_back(entity);

		selectedInfo->setSelectedType(entityType);
		selectedInfo->setAllNumber(selected.size());
		selectedInfo->select(entity);
	}
}

void Controls::select(const std::vector<Physical*>* entities) {
	auto player = Game::getPlayersMan()->getActivePlayerID();
	for (const auto physical : *entities) {
		selectOne(physical, player);
	}
	updateAdditionalInfo();
}

void Controls::leftClick(hit_data& hitData) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	if (hitData.isGround) {
		unSelectAll();
	} else {
		selectOne(hitData.clicked, Game::getPlayersMan()->getActivePlayerID());
	}
	updateAdditionalInfo();
}

void Controls::leftDoubleClick(hit_data& hitData) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(Game::getEnvironment()->getNeighboursSimilarAs(hitData.clicked));
}

void Controls::leftClickBuild(hit_data& hitData) {
	unSelectAll();
	createBuilding({hitData.position.x_, hitData.position.z_});
}

UnitOrder* Controls::vectorOrder(UnitAction order, Urho3D::Vector2* vector, const bool shiftPressed,
                                 const std::vector<Physical*>& vec) const {
	if (vec.size() == 1) {
		return new IndividualOrder(static_cast<Unit*>(vec.at(0)), order, *vector, shiftPressed);
	}
	return new GroupOrder(vec, UnitActionType::ORDER, static_cast<short>(order), *vector, shiftPressed);
}

UnitOrder* Controls::thingOrder(UnitAction order, Physical* toUse, const bool shiftPressed,
                                const std::vector<Physical*>& vec) const {
	if (vec.size() == 1) {
		return new IndividualOrder(static_cast<Unit*>(vec.at(0)), order, toUse, shiftPressed);
	}
	return new GroupOrder(vec, UnitActionType::ORDER, static_cast<short>(order), toUse, shiftPressed);
}

UnitAction Controls::chooseAction(hit_data& hitData) const {
	return hitData.clicked->getTeam() == selected.at(0)->getTeam()
		       ? UnitAction::FOLLOW
		       : UnitAction::ATTACK;
}

UnitOrder* Controls::createUnitOrder(hit_data& hitData) const {
	const bool shiftPressed = input->GetKeyDown(Urho3D::KEY_SHIFT);
	switch (hitData.getType()) {
	case ObjectType::NONE:
		return vectorOrder(UnitAction::GO, new Urho3D::Vector2(hitData.position.x_, hitData.position.z_), shiftPressed,
		                   selected);
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return thingOrder(chooseAction(hitData), hitData.clicked, shiftPressed, selected);
	case ObjectType::RESOURCE:
		return thingOrder(UnitAction::COLLECT, hitData.clicked, shiftPressed, selected);
	default: ;
	}
}

void Controls::rightClick(hit_data& hitData) const {
	Game::getActionCenter()->addUnitAction(createUnitOrder(hitData));
}

void Controls::leftHold(MouseHeld& held) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}

	select(Game::getEnvironment()->getNeighbours(held, Game::getPlayersMan()->getActivePlayerID()));
}

void Controls::rightHold(MouseHeld& held) const {
	GroupOrder* first = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::GO),
	                                   held.firstAs2D());
	GroupOrder* second;
	if (input->GetKeyDown(Urho3D::KEY_SHIFT)) {
		second = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::GO),
		                        held.secondAs2D(), true);
	} else {
		second = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::CHARGE),
		                        held.first2Second(), true); //TODO buf append nie dzia³a
	}

	Game::getActionCenter()->addUnitAction(first, second);
}

void Controls::releaseLeft() {
	hit_data hitData;

	if (raycast(hitData)) {
		left.setSecond(hitData.position);
		const float dist = left.sq2DDist();

		if (left.timeUpDiff() < 0.2f && dist < clickDistance) {
			leftDoubleClick(hitData);
		} else if (dist > clickDistance) {
			leftHold(left.held);
		} else if (hitData.isSth()) {
			leftClick(hitData);
		}
	}
	Game::getEnvironment()->setTerrainShaderParam("SelectionEnable", false);
}

void Controls::releaseRight() {
	hit_data hitData;

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycast(hitData)) {
		right.setSecond(hitData.position);
		if (right.sq2DDist() > clickDistance) {
			rightHold(right.held);
		} else if (hitData.isSth()) {
			rightClick(hitData);
		}
	}
	arrowNode->SetEnabled(false);
}

void Controls::releaseBuildLeft() {
	hit_data hitData;

	if (raycastGround(hitData)) {
		leftClickBuild(hitData);
		if (input->GetKeyDown(Urho3D::KEY_CTRL)) {
			cleanMouse();
		} else {
			toDefault();
		}
	}
}

bool Controls::orderAction() const {
	hit_data hitData;

	//if (raycast(hitData) && hitData.clicked) {
	if (raycast(hitData)) {
		rightClick(hitData);
		return true;
	}
	return false;
}

void Controls::toBuild(HudData* hud) {
	state = ControlsState::BUILD;
	typeToCreate = ObjectType::BUILDING;
	idToCreate = hud->getId();
	tempBuildingNode->SetEnabled(false);
	resetCircles();
}

BuildingActionType Controls::getBuildingActionType(ActionType type) {
	switch (type) {
	case ActionType::UNIT_CREATE:
		return BuildingActionType::UNIT_CREATE;
	case ActionType::UNIT_LEVEL:
		return BuildingActionType::UNIT_LEVEL;
	case ActionType::UNIT_UPGRADE:
		return BuildingActionType::UNIT_UPGRADE;
	default: ;
	}
}

void Controls::order(short id, ActionType type) {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::NONE:
		return Game::getActionCenter()->add(
		                                    new GeneralActionCommand(id, GeneralActionType::BUILDING_LEVEL,
		                                                             Game::getPlayersMan()->getActivePlayerID()));
	case ObjectType::UNIT:
		return actionUnit(id, type);
	case ObjectType::BUILDING:
		return executeOnBuildings(getBuildingActionType(type), id);
	case ObjectType::RESOURCE:
		return executeOnResources(ResourceActionType(id));
	}
}

void Controls::executeOnUnits(short id) const {
	Game::getActionCenter()->addUnitAction(new GroupOrder(selected, UnitActionType::ORDER, id, nullptr, false));
}

void Controls::executeOnResources(ResourceActionType action) const {
	Game::getActionCenter()->add(
	                             new ResourceActionCommand(selected, action,
	                                                       Game::getPlayersMan()->getActivePlayerID()));
}

void Controls::executeOnBuildings(BuildingActionType action, short id) const {
	Game::getActionCenter()->add(new BuildingActionCommand(selected, action, id));
}

bool Controls::clickDown(MouseButton& var) const {
	hit_data hitData;
	if (raycast(hitData)) {
		var.setFirst(hitData.position);
		return true;
	}
	return false;
}

void Controls::createBuilding(Urho3D::Vector2 pos) const {
	if (idToCreate >= 0) {
		auto player = Game::getPlayersMan()->getActivePlayerID();
		Game::getActionCenter()->addBuilding(idToCreate, pos, player, false);
	}
}

void Controls::cleanMouse() {
	arrowNode->SetEnabled(false);

	Game::getEnvironment()->setTerrainShaderParam("SelectionEnable", false);
}

void Controls::deactivate() {
	if (active) {
		active = false;
		cleanMouse();
	}
}

void Controls::activate() {
	if (!active) {
		active = true;
		cleanMouse();
	}
}

void Controls::unitOrder(short id) {
	const auto type = UnitAction(id);
	switch (type) {
	case UnitAction::GO:
	case UnitAction::CHARGE:
	case UnitAction::ATTACK:
	case UnitAction::FOLLOW:
		state = ControlsState::ORDER;
		unitOrderType = type;
		break;
	case UnitAction::STOP:
	case UnitAction::DEFEND:
	case UnitAction::DEAD:
		executeOnUnits(id);
		break;
	default: ;
	}
}

void Controls::actionUnit(short id, ActionType type) {
	switch (type) {
	case ActionType::ORDER:
		return unitOrder(id);
	case ActionType::FORMATION:
		return unitFormation(id);
	}
}

void Controls::refreshSelected() {
	const int sizeBefore = selected.size();
	//TODO use std::stable_partition
	selected.erase(
	               std::remove_if(
	                              selected.begin(), selected.end(),
	                              [](Physical* physical) {
		                              if (!physical->isAlive()) {
			                              physical->unSelect();
			                              return true;
		                              }
		                              return false;
	                              }),
	               selected.end());

	if (sizeBefore != selected.size()) {
		//TODO perf nie koniecznie resetowac calosc
		selectedInfo->refresh(selected);
	}
}

bool Controls::conditionToClean(const FrameInfo* frameInfo) const {
	if (!frameInfo->isRealFrame()) {
		return false;
	}
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::UNIT:
		return StateManager::isUnitDead();
	case ObjectType::BUILDING:
		return StateManager::isBuildingDead();
	case ObjectType::RESOURCE:
		return StateManager::isResourceDead();
	}
	return false;
}

void Controls::cleanAndUpdate(const FrameInfo* frameInfo) {
	if (conditionToClean(frameInfo)) {
		refreshSelected();
	}
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::UNIT:
		for (int i = 0; i < Urho3D::Min(selected.size(), 5); ++i) {
			const auto ent = selected.at(i);
			setCircleSight(i, ent->getPosition(), ent->getSightRadius(), Urho3D::Color::MAGENTA);
		}
		break;
	case ObjectType::RESOURCE:
		break;
	case ObjectType::BUILDING:
		for (int i = 0; i < Urho3D::Min(selected.size(), 5); ++i) {
			const Building* build = (Building*)selected.at(i);

			setCircleSight(i, build->getPosition(), getCircleSize(build->getDb(), build->getLevel()),
			               Game::getColorPaletteRepo()->getCircleColor(build->getDb()));
		}
	}
}

void Controls::updateSelection() const {
	hit_data hitData;

	if (raycastGround(hitData)) {
		const float xScale = left.held.first.x_ - hitData.position.x_;
		const float zScale = left.held.first.z_ - hitData.position.z_;
		if ((xScale * xScale > clickDistance || zScale * zScale > clickDistance)
			&& Game::getTime() - left.lastDown > 0.3f) {
			Game::getEnvironment()->setTerrainShaderParam("SelectionEnable", true);

			auto [minX, maxX] = std::minmax(left.held.first.x_, hitData.position.x_);
			auto [minZ, maxZ] = std::minmax(left.held.first.z_, hitData.position.z_);

			Game::getEnvironment()->setTerrainShaderParam("SelectionRect",
			                                              Urho3D::Vector4(minX, minZ, maxX, maxZ));
		} else {
			Game::getEnvironment()->setTerrainShaderParam("SelectionEnable", false);
		}
	}
}

void Controls::updateArrow() const {
	hit_data hitData;

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycastGround(hitData)) {
		auto dir = right.held.first - hitData.position;

		const float length = dir.Length();

		if (length * length > clickDistance && Game::getTime() - right.lastDown > 0.3f) {
			if (!arrowNode->IsEnabled()) {
				arrowNode->SetEnabled(true);
			}
			arrowNode->SetScale({length, 1, length / 3});
			arrowNode->SetDirection({-dir.z_, 0, dir.x_});
			arrowNode->SetPosition(right.held.first);
		} else if (arrowNode->IsEnabled()) {
			arrowNode->SetEnabled(false);
		}
	} else if (arrowNode->IsEnabled()) {
		arrowNode->SetEnabled(false);
	}
}

void Controls::toDefault() {
	state = ControlsState::DEFAULT;
	cleanMouse();
	idToCreate = -1;
	tempBuildingNode->SetEnabled(false);
	resetCircles();
}

void Controls::unitFormation(short id) const {
	Game::getActionCenter()->addUnitAction(
	                                       new GroupOrder(selected, UnitActionType::FORMATION, id, nullptr));
}

void Controls::control() {
	switch (state) {
	case ControlsState::DEFAULT:
		return defaultControl(); //TODO bug gdy zaznaczony jest resource to probuje storzyc formacje przy chargfe
	case ControlsState::BUILD:
		return buildControl();
	case ControlsState::ORDER:
		return orderControl();
	}
}

void Controls::defaultControl() {
	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		if (!left.isHeld) {
			clickDown(left);
		} else {
			updateSelection();
		}
	} else if (left.isHeld) {
		releaseLeft();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		if (!right.isHeld) {
			clickDown(right);
		} else {
			updateArrow();
		}
	} else if (right.isHeld) {
		releaseRight();
	}

	if (input->GetMouseButtonPress(Urho3D::MOUSEB_MIDDLE)) {
		hit_data hitData;

		if (raycast(hitData)) {
			if (hitData.clicked) {
				Game::getLog()->Write(3, hitData.clicked->getInfo());
			}
			Game::getLog()->Write(3, hitData.position.ToString());
		}
	}
}

void Controls::buildControl() {
	if (!input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT) && !input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		hit_data hitData;

		if (raycastGround(hitData)) {
			//TODO perf nie robic tego co klatke
			auto env = Game::getEnvironment();

			const auto building = Game::getDatabase()->getBuilding(idToCreate);

			auto hitPos = Urho3D::Vector2(hitData.position.x_, hitData.position.z_);
			auto level = Game::getPlayersMan()->getActivePlayer()->getLevelForBuilding(idToCreate);
			tempBuildingNode->SetPosition(env->getValidPosition(building->size, hitPos));
			if (!tempBuildingNode->IsEnabled()) {
				tempBuildingNode->LoadXML(Game::getCache()
				                          ->GetResource<Urho3D::XMLFile>("Objects/buildings/" + level->nodeName)
				                          ->GetRoot());
				const auto model = tempBuildingNode->GetComponent<Urho3D::StaticModel>();
				for (int i = 0; i < model->GetNumGeometries(); ++i) {
					model->SetMaterial(i, model->GetMaterial(i)->Clone());
				}
				tempBuildingNode->SetEnabled(true);
			}

			setCircleSight(0, tempBuildingNode->GetPosition(), getCircleSize(building, level),
			               Game::getColorPaletteRepo()->getCircleColor(building));

			setShaderParam(tempBuildingNode, "MatDiffColor",
			               Game::getColorPaletteRepo()->getColorForValidation(building, hitPos));
		}
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		left.markHeld();
	} else if (left.isHeld) {
		releaseBuildLeft();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		toDefault();
	}
}

void Controls::orderControl() {
	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		left.markHeld();
	} else if (left.isHeld) {
		switch (unitOrderType) {
		case UnitAction::GO:
		case UnitAction::ATTACK:
		case UnitAction::FOLLOW:
			if (orderAction()) {
				toDefault();
			}
			break;
		case UnitAction::CHARGE:
			break;
		default: ;
		}
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		right.markHeld();
	} else if (right.isHeld) {
		toDefault();
	}
}
