#include "ResourceEntity.h"
#include "Game.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include "database/DatabaseCache.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <string>

float ResourceEntity::hbMaxSize = 3.0f;

ResourceEntity::ResourceEntity(Vector3* _position, int id, int level) : Static(_position, ObjectType::RESOURCE) {
	hbMaxSize = 3.0;
	db_resource* dbResource = Game::get()->getDatabaseCache()->getResource(id);
	const String modelName = "Models/" + dbResource->model;
	populate(dbResource);
	node->Scale(dbResource->scale);
	StaticModel* model = node->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>(modelName));
	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		model->SetMaterial(i, Game::get()->getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
	}
	initBillbords();
}

ResourceEntity::~ResourceEntity() = default;

int ResourceEntity::getDbID() {
	return dbResource->id;
}

void ResourceEntity::populate(db_resource* _dbResource) {
	type = _dbResource->id;
	gridSize = _dbResource->size;
	amonut = _dbResource->maxCapacity;
	maxUsers = _dbResource->maxUsers;
	dbResource = _dbResource;
}

float ResourceEntity::getHealthBarSize() {
	return hbMaxSize;
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

double ResourceEntity::collect(double collectSpeed) {
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		return collectSpeed;
	}
	const double toReturn = amonut;
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
