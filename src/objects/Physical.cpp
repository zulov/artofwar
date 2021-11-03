#include "objects/Physical.h"

#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include "Game.h"
#include "SelectedObject.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/dbload_container.h"


Physical::Physical(Urho3D::Vector3& _position, bool withNode):
	position(_position) {
	if (withNode) {
		node = Game::getScene()->CreateChild();
		node->SetVar("link", this);
		node->SetPosition(position);
	}

	shouldUpdate = true;
}

Physical::~Physical() {
	if (node) {
		node->Remove();
	}

	unSelect();
}

void Physical::clearNodeWithOutDelete() {
	node = nullptr;
}

bool Physical::isAlive() const {
	return true;
}

float Physical::calculateAuraSize(const Urho3D::Vector3& boundingBox) const {
	return (boundingBox.x_ + boundingBox.z_) / 2 * 1.2f;
}

void Physical::updateBillboards() const {
	if (selectedObject) {
		auto const healthBar = selectedObject->getHealthBar();
		if (healthBar) {
			healthBar->position_ = node->GetPosition() + Urho3D::Vector3{0, getModelHeight() * 1.3f, 0};
			healthBar->size_ = {getHealthBarSize(), getHealthBarThick()};
			healthBar->enabled_ = true;
		}
		auto const aura = selectedObject->getAura();
		if (aura) {
			aura->position_ = node->GetPosition();
			float aS = getAuraSize();
			aura->size_ = {aS, aS};
			aura->enabled_ = true;
		}
	}
}

void Physical::updateHealthBar() const {
	if (selectedObject) {
		auto const healthBar = selectedObject->getHealthBar();
		if (healthBar) {
			healthBar->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick());
		}
	}
}

std::optional<Urho3D::Vector2> Physical::getPosToUseBy(Unit* follower) {
	auto a = getPosToUseWithDist(follower);
	if (a.has_value()) {
		return std::get<0>(a.value());
	}
	return {};
}

float Physical::getHealthBarSize() const {
	const auto healthBarSize = getMaxHpBarSize() * getHealthPercent();
	return healthBarSize <= 0.f ? 0.f : healthBarSize;
}

bool Physical::isSelected() const {
	return selectedObject;
}

void Physical::load(dbload_physical* dbloadPhysical) {
	hp = dbloadPhysical->hp;
}

char Physical::getLevelNum() {
	return -1;
}

std::string Physical::getValues(int precision) {
	int hp_coef = hp * precision;
	return std::to_string(getId()) + ","
		+ std::to_string(hp_coef) + ","
		+ std::to_string(player) + ","
		+ std::to_string(getLevelNum()) + ",";
}

int Physical::belowCloseLimit() const {
	return Urho3D::Max(getMaxCloseUsers() - closeUsers, 0);
}

void Physical::select(SelectedObject* selectedObject) {
	assert(this->selectedObject == nullptr);

	this->selectedObject = selectedObject;

	updateBillboards();
	updateHealthBar();
}


void Physical::unSelect() {
	if (selectedObject) {
		selectedObject->disableBillboards();
		selectedObject = nullptr;
	}
}

void Physical::setVisibility(bool value) {
	if (node->IsEnabled() != value) {
		node->SetEnabled(value);
	}
}

void Physical::loadXml(const Urho3D::String& xmlName) {
	Urho3D::String name;
	if (SIM_GLOBALS.HEADLESS) {
		name = "";
	}
		// else if (SIM_GLOBALS.TRAIN_MODE) {
		// 	name = "Objects/mock.xml";
		// }
	else {
		name = xmlName;
	}

	//node->RemoveAllChildren();
	if (!name.Empty()) {
		node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(name)->GetRoot());
		node->SetVar("link", this);
		if (getModelHeight() < 0.f) {
			const auto model = node->GetComponent<Urho3D::StaticModel>();
			const auto boundingBox = model->GetModel()->GetBoundingBox().Size() * node->GetScale();

			setModelData(Urho3D::Max(1.f, boundingBox.y_), Urho3D::Max(1.f, calculateAuraSize(boundingBox)));
		}
	}

	populate();
	updateBillboards();
}


void Physical::setPlayerAndTeam(int player) {
	setPlayer(player);
	setTeam(Game::getPlayersMan()->getPlayer(player)->getTeam());
}
