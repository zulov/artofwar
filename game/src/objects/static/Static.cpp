#include "Static.h"

#include <magic_enum.hpp>
#include <string>
#include "Game.h"
#include "math/MathUtils.h"
#include "../unit/Unit.h"
#include "database/db_strcut.h"
#include "objects/NodeUtils.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "utils/OtherUtils.h"

Static::Static(Urho3D::Vector3& _position, int indexInGrid, UId uId) : Physical(_position, uId) {
	Static::setBucketInMainGrid(indexInGrid);
}

Static::~Static() {
	delete []data;
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

bool Static::isInCloseRange(int index) const {
	auto cells = getSurroundCells();
	return std::ranges::find(cells, index) != cells.end();
}

Urho3D::Color Static::getColor(db_player_colors* col) const {
	return col->buildingColor;
}

void Static::setVisibility(VisibilityType type) {
	if (node && type != visibilityType) {
		switch (type) {
		case VisibilityType::NONE:
			if (node->IsEnabled()) {
				node->SetEnabled(false);
			}
			break;
		case VisibilityType::SEEN:
			if (!node->IsEnabled()) {
				node->SetEnabled(true);
			}
			setShaderParam(this, "SemiHide", true);
			break;
		case VisibilityType::VISIBLE: {
			if (!node->IsEnabled()) {
				node->SetEnabled(true);
			}
			setShaderParam(this, "SemiHide", false);
		}
		break;
		}
	}
	visibilityType = type;
}

Urho3D::IntVector2 Static::getSurroundSize(const Urho3D::IntVector2 oSize, int res) {
	auto result = oSize + Urho3D::IntVector2(-1, 1);
	if (result.x_ < 0) {
		result.x_ = 0;
	}
	if (result.y_ >= res) {
		result.y_ = res - 1;
	}

	return result;
}

void Static::populate() {
	auto env = Game::getEnvironment();
	const auto gridSize = getGridSize();
	const auto cordsCell = env->getCords(indexInMainGrid);
	const auto oSizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto oSizeZ = calculateSize(gridSize.y_, cordsCell.y_);
	const auto res = env->getResolution();
	const auto sSizeX = getSurroundSize(oSizeX, res);
	const auto sSizeZ = getSurroundSize(oSizeZ, res);

	occupiedCellsSize = gridSize.x_ * gridSize.y_;

	auto tabSize = (sSizeX.y_ - sSizeX.x_) * (sSizeZ.y_ - sSizeZ.x_);
	data = new int[tabSize]; //TODO obliczyc dokladnie rozmiar
	int* o = data;
	int* s = data + occupiedCellsSize;
	for (short i = sSizeX.x_; i < sSizeX.y_; ++i) {
		const auto index = env->getIndex(i, 0);
		for (short j = sSizeZ.x_; j < sSizeZ.y_; ++j) {
			if (i >= oSizeX.x_ && i < oSizeX.y_ && j >= oSizeZ.x_ && j < oSizeZ.y_) {
				*o = index + j;
				o++;
			} else {
				surroundCellsSize++;
				*s = index + j;
				s++;
			}
		}
	}
	assert(surroundCellsSize + occupiedCellsSize == tabSize);
}

int Static::belowCloseLimit() const {
	const auto freeClose = Physical::belowCloseLimit();
	if (freeClose <= 0) {
		return 0;
	}
	return Urho3D::Min(freeClose, hasFreeSpace());
}

bool Static::hasAnyFreeSpace() const {
	return std::ranges::any_of(getSurroundCells(), [this](int index) { return canUse(index); });
}

int Static::hasFreeSpace() const {
	return std::ranges::count_if(getSurroundCells(), [this](int index) { return canUse(index); });
}

std::optional<std::tuple<Urho3D::Vector2, float>> Static::getPosToUseWithDist(Unit* user) {
	float minDistance = 9999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;

	for (auto index : getSurroundCells()) {
		if (canUse(index)) {
			const auto vec = Game::getEnvironment()->getCenter(index);
			setClosest(minDistance, closest, closestIndex, index, vec, user->getPosition());
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance}};
	}
	return {};
}

std::vector<int> Static::getIndexesForUse() const {
	std::vector<int> indexes;
	if (belowCloseLimit() <= 0) { return indexes; }
	indexes.reserve(surroundCellsSize);
	addIndexesForUse(indexes);

	return indexes;
}

void Static::addIndexesForUse(std::vector<int>& indexes) const {
	for (auto index : getSurroundCells()) {
		if (canUse(index)) {
			indexes.push_back(index);
		}
	}
}

bool Static::indexCanBeUse(int index) const {
	if (!canUse(index)) { return false; }

	return std::ranges::any_of(getSurroundCells(), [&](const auto& i) { return index == i; });
}

std::vector<int> Static::getIndexesForRangeUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowRangeLimit() <= 0) { return indexes; }

	const std::vector<int> allIndexes = Game::getEnvironment()->getIndexesInRange(
		 getPosition(), user->getLevel()->attackRange);
	auto cells = getOccupiedCells();

	for (auto index : allIndexes) {
		if (canUse(index)
			&& !isInCloseRange(index)
			&& std::ranges::find(cells, index) == cells.end()) {
			indexes.push_back(index);
		}
	}
	return indexes;
}

std::string Static::getValues(int precision) {
	const auto cordsCell = Game::getEnvironment()->getCords(indexInMainGrid);
	return Physical::getValues(precision)
		+ std::to_string(cordsCell.x_) + ","
		+ std::to_string(cordsCell.y_) + ","
		+ std::to_string(cast(state)) + ","
		+ std::to_string(cast(nextState));
}
