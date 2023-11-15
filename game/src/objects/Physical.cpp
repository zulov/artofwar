#include "objects/Physical.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include "Game.h"
#include "NodeUtils.h"
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
}

Physical::~Physical() {
	if (node) {
		node->Remove();
		node = nullptr;
	}
	clearSelection();
}

void Physical::clearNodeWithOutDelete() {
	node = nullptr;
}

bool Physical::isAlive() const {
	return true;
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
	return selected;
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

void Physical::select() {
	assert(!selected);

	selected = true;

	setShaderParam(this, "OutlineEnable", true);
}


void Physical::clearSelection() {
	selected = false;
}

void Physical::unSelect() {
	setShaderParam(this, "OutlineEnable", false);

	clearSelection();
}

bool Physical::isNodeEnabled() const {
	return node && node->IsEnabled();
}

void Physical::setDefaultShader(Urho3D::Material* mat) const {
	mat->SetShaderParameter("OutlineEnable", false);
	mat->SetShaderParameter("SemiHide", false);
	mat->SetShaderParameter("ColorPercent", 2.0);
	mat->SetShaderParameter("Progress", 2.0);
	mat->SetShaderParameter("VerticalPos",
	                        Urho3D::Vector2(getPosition().y_, getPosition().y_ + getModelHeight()));
}

void Physical::loadXml(const Urho3D::String& xmlName) {
	if (!SIM_GLOBALS.HEADLESS) {
		node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(xmlName)->GetRoot());

		const auto model = node->GetComponent<Urho3D::StaticModel>();
		if (getModelHeight() < 0.f) {
			const auto boundingBox = model->GetModel()->GetBoundingBox().Size() * node->GetScale();

			setModelData(Urho3D::Max(1.f, boundingBox.y_));
		}

		for (int i = 0; i < model->GetNumGeometries(); ++i) {
			setDefaultShader(model->GetMaterial(i));
		}
	}
}

void Physical::setPlayerAndTeam(int player) {
	setPlayer(player);
	setTeam(Game::getPlayersMan()->getPlayer(player)->getTeam());
}

void Physical::ensureMaterialCloned() {
	if (node && !materialCloned) {
		const auto model = node->GetComponent<Urho3D::StaticModel>();
		for (int i = 0; i < model->GetNumGeometries(); ++i) {
			model->SetMaterial(i, model->GetMaterial(i)->Clone());
			auto mat = model->GetMaterial(i);
			setDefaultShader(mat);
			if (player >= 0 && mat->GetShaderParameter("OutlineColor").IsEmpty()) {
				auto colorId = Game::getPlayersMan()->getPlayer(player)->getColor();
				db_player_colors* col = Game::getDatabase()->getPlayerColor(colorId);
				mat->SetShaderParameter("OutlineColor", getColor(col));
			}
		}
		materialCloned = true;
	}
}

void Physical::setIndexInInfluence(int index) {
	assert(index >= -1);
	indexInInfluence = index;
}
