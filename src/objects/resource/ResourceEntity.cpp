#include "ResourceEntity.h"
#include "Game.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>

double ResourceEntity::hbMaxSize = 3.0;

ResourceEntity::ResourceEntity(Vector3* _position, int id) : Static(_position, RESOURCE) {
	hbMaxSize = 3.0;
	db_resource* dbResource = Game::get()->getDatabaseCache()->getResource(id);
	String modelName = "Models/" + dbResource->model;
	populate(dbResource);
	node->Scale(dbResource->scale);
	StaticModel* model = node->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>(modelName));
	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		model->SetMaterial(i, Game::get()->getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
	}
	initBillbords();
}

ResourceEntity::~ResourceEntity() {
	if (name) {
		delete name;
	}
}

int ResourceEntity::getID() {
	return dbResource->id;
}

void ResourceEntity::populate(db_resource* _dbResource) {
	type = _dbResource->id;
	name = new String(_dbResource->name);
	gridSize = _dbResource->size;
	amonut = _dbResource->maxCapacity;
	maxUsers = _dbResource->maxUsers;
	dbResource = _dbResource;
	//plane->SetScale(Vector3(_dbResource->sizeX, 1, _dbResource->sizeZ));
}

double ResourceEntity::getHealthBarSize() {
	return hbMaxSize;
}

String* ResourceEntity::toMultiLineString() {
	(*menuString) = dbResource->name;
	(*menuString) += "\nZasobów: " + String(amonut);
	return menuString;
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
