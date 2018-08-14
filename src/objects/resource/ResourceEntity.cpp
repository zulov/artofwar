#include "ResourceEntity.h"
#include "Game.h"
#include "MathUtils.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "simulation/env/Enviroment.h"
#include "objects/unit/state/StateManager.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>


ResourceEntity::
ResourceEntity(Urho3D::Vector3* _position, int id, int level, int mainCell)
	: Static(_position, ObjectType::RESOURCE, mainCell) {
	initBillbords();

	dbResource = Game::getDatabaseCache()->getResource(id);
	populate();

	auto model = node->CreateComponent<Urho3D::StaticModel>();
	model->SetModel(Game::getCache()->GetResource<Urho3D::Model>("Models/" + dbResource->model));

	node->Scale(dbResource->scale);

	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		model->SetMaterial(i, Game::getCache()->GetResource<Urho3D::Material>("Materials/" + dbResource->texture[i]));
	}
	updateBillbords();
}

ResourceEntity::~ResourceEntity() = default;

int ResourceEntity::getDbID() {
	return dbResource->id;
}

void ResourceEntity::populate() {
	Static::populate(dbResource->size);

	amonut = dbResource->maxCapacity;
	maxCloseUsers = dbResource->maxUsers;
}

float ResourceEntity::getMaxHpBarSize() {
	return 3.0;
}

float ResourceEntity::getHealthPercent() const {
	return amonut / dbResource->maxCapacity;
}

Urho3D::String& ResourceEntity::toMultiLineString() {
	menuString = dbResource->name;
	menuString += "\nZasobów: " + Urho3D::String(amonut)
		+ "\nU¿ytkowników: " + Urho3D::String((int)closeUsers)
		+ "/" + Urho3D::String((int)maxCloseUsers);
	return menuString;
}

std::string ResourceEntity::getValues(int precision) {
	int amountI = amonut * precision;
	return Static::getValues(precision)
		+ std::to_string(amountI);
}

std::string ResourceEntity::getColumns() {
	return Static::getColumns() +
		"amount		INT     NOT NULL";
}

float ResourceEntity::collect(float collectSpeed) {
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		updateHealthBar();
		return collectSpeed;
	}
	const float toReturn = amonut;
	amonut = 0;
	StateManager::changeState(this, StaticState::DEAD);
	return toReturn;
}
