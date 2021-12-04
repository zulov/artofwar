#include "objects/Physical.h"

#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include "Game.h"
#include "NodeUtils.h"
#include "SelectedObject.h"
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
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

void Physical::updateBillboards() const {
	if (selectedObject) {
		auto const healthBar = selectedObject->getHealthBar();
		if (healthBar) {
			healthBar->position_ = node->GetPosition() + Urho3D::Vector3{0, getModelHeight() * 1.3f, 0};
			healthBar->size_ = {getHealthBarSize(), getHealthBarThick()};
			healthBar->enabled_ = true;
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

	setShaderParam(node, "OutlineEnable", true);

	updateBillboards();
	updateHealthBar();
}


void Physical::unSelect() {
	setShaderParam(node, "OutlineEnable", false);

	if (selectedObject) {
		selectedObject->disableBillboards();
		selectedObject = nullptr;
	}
}


void Physical::loadXml(const Urho3D::String& xmlName) {
	Urho3D::String name;
	if (SIM_GLOBALS.HEADLESS) {
		name = "";
	} else {
		name = xmlName;
	}

	//node->RemoveAllChildren();
	if (!name.Empty()) {
		node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(name)->GetRoot());
		node->SetVar("link", this);
		const auto model = node->GetComponent<Urho3D::StaticModel>();
		if (getModelHeight() < 0.f) {
			const auto boundingBox = model->GetModel()->GetBoundingBox().Size() * node->GetScale();

			setModelData(Urho3D::Max(1.f, boundingBox.y_));
		}

		for (int i = 0; i < model->GetNumGeometries(); ++i) {
			model->SetMaterial(i, model->GetMaterial(i)->Clone()); //TODO memory
			auto mat = model->GetMaterial(i);
			mat->SetShaderParameter("OutlineEnable", false);
			mat->SetShaderParameter("SemiHide", false);
			mat->SetShaderParameter("Progress", 2.0);
			if (mat->GetShaderParameter("OutlineColor").IsEmpty()) {
				if (player >= 0) {
					auto colorId = Game::getPlayersMan()->getPlayer(player)->getColor();
					db_player_colors* col = Game::getDatabase()->getPlayerColor(colorId);
					mat->SetShaderParameter("OutlineColor", getColor(col));
				}
			}
		}
	}

	populate();
	updateBillboards();
}


void Physical::setPlayerAndTeam(int player) {
	setPlayer(player);
	setTeam(Game::getPlayersMan()->getPlayer(player)->getTeam());
}
