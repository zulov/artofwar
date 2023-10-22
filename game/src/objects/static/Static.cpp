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

Static::Static(Urho3D::Vector3& _position, int indexInGrid, bool withNode) : Physical(_position, withNode),
                                                                             state(StaticState::ALIVE),
                                                                             nextState(StaticState::ALIVE) {
	this->indexInMainGrid = indexInGrid;
}

Static::~Static() {
	delete []data;
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

void Static::setIndexInInfluence(int index) {
	assert(index < std::numeric_limits<unsigned short>::max());
	assert(index >= 0);
	indexInInfluence = index;
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

void Static::populate() {
	const auto gridSize = getGridSize();
	const auto cordsCell = Game::getEnvironment()->getCords(indexInMainGrid);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_) + Urho3D::IntVector2(-1, 1);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_) + Urho3D::IntVector2(-1, 1);
	const auto res = Game::getEnvironment()->getResolution();
	occupiedCellsSize = gridSize.x_ * gridSize.y_;

	data = new int[(gridSize.x_ + 2) * (gridSize.y_ + 2)];//TODO obliczyc dokladnie rozmiar
	int* o = data;
	int* s = data + occupiedCellsSize;
	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		if (i < 0 || i >= res) { continue; }
		const auto index = Game::getEnvironment()->getIndex(i, 0);
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			if (j < 0 || j >= res) { continue; }//TODO perf nie iterowac tylko tam gdzie nie wykracza
			if (i == sizeX.x_ || i == sizeX.y_ - 1 || j == sizeZ.x_ || j == sizeZ.y_ - 1) {
				surroundCellsSize++;
				*s = index + j;
				s++;
			} else {
				*o = index + j;
				o++;
			}
		}
	}
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

std::vector<int> Static::getIndexesForUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowCloseLimit() <= 0) { return indexes; }
	indexes.reserve(surroundCellsSize);
	for (auto index : getSurroundCells()) {
		if (canUse(index)) {
			indexes.push_back(index);
		}
	}

	return indexes;
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
		+ std::to_string(cast(nextState)) + ",";
}
