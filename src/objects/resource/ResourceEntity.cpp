#include "ResourceEntity.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>


ResourceEntity::ResourceEntity(Vector3* _position, int id, int level) : Static(_position, ObjectType::RESOURCE) {
	initBillbords();

	dbResource = Game::get()->getDatabaseCache()->getResource(id);

	populate();

	StaticModel* model = node->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>("Models/" + dbResource->model));

	node->Scale(dbResource->scale);

	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		model->SetMaterial(i, Game::get()->getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
	}
	updateBillbords();
}

ResourceEntity::~ResourceEntity() = default;

int ResourceEntity::getDbID() {
	return dbResource->id;
}

void ResourceEntity::populate() {
	type = dbResource->id;
	gridSize = dbResource->size;
	amonut = dbResource->maxCapacity;
	maxUsers = dbResource->maxUsers;
}

float ResourceEntity::getMaxHpBarSize() {
	return 3.0;
}

String& ResourceEntity::toMultiLineString() {
	menuString = dbResource->name;
	menuString += "\nZasobów: " + String(amonut);
	return menuString;
}

std::string ResourceEntity::getValues(int precision) {
	int amountI = amonut * precision;
	return Static::getValues(precision)
		+ to_string(amountI);
}

std::string ResourceEntity::getColumns() {
	return Static::getColumns() +
		"amount		INT     NOT NULL";
}

float ResourceEntity::collect(float collectSpeed) {
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		return collectSpeed;
	}
	const float toReturn = amonut;
	amonut = 0;
	return toReturn;
}

bool ResourceEntity::belowLimit() {
	return users < maxUsers;
}

void ResourceEntity::up() {
	++users;
}

void ResourceEntity::reduce() {
	--users;
}
