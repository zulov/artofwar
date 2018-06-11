#include "ResourceEntity.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "ObjectEnums.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"


ResourceEntity::ResourceEntity(Vector3* _position, int id, int level) : Static(_position, ObjectType::RESOURCE) {
	initBillbords();

	dbResource = Game::getDatabaseCache()->getResource(id);

	populate();

	StaticModel* model = node->CreateComponent<StaticModel>();
	model->SetModel(Game::getCache()->GetResource<Model>("Models/" + dbResource->model));

	node->Scale(dbResource->scale);

	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		model->SetMaterial(i, Game::getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
	}
	updateBillbords();
}

ResourceEntity::~ResourceEntity() = default;

int ResourceEntity::getDbID() {
	return dbResource->id;
}

bool ResourceEntity::isAlive() const {
	return amonut > 0;
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

bool ResourceEntity::hasFreeSpace() const {
	const IntVector2 sizeX = calculateSize(gridSize.x_, mainCell.x_);
	const IntVector2 sizeZ = calculateSize(gridSize.y_, mainCell.y_);
	auto env = Game::getEnviroment();

	for (short i = sizeX.x_ - 1; i < sizeX.y_ + 1; ++i) {
		for (short j = sizeZ.x_ - 1; j < sizeZ.y_ + 1; ++j) {
			env->get
		}
	}
}

std::string ResourceEntity::getColumns() {
	return Static::getColumns() +
		"amount		INT     NOT NULL";
}

bool ResourceEntity::belowCloseLimit() const {
	return Physical::belowCloseLimit()
		&& hasFreeSpace();
}

float ResourceEntity::collect(float collectSpeed) {
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		updateHealthBar();
		return collectSpeed;
	}
	const float toReturn = amonut;
	amonut = 0;
	return toReturn;
}
