#include "InfluenceMapCombine.h"


#include "Game.h"
#include "objects/Physical.h"
#include "player/PlayersManager.h"
#include "utils/DeleteUtils.h"

InfluenceMapCombine::InfluenceMapCombine(unsigned short resolution, float size, float coef, char level,
                                         float valueThresholdDebug) :
	InfluenceMap(resolution, size, valueThresholdDebug), coef(coef), level(level) {
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		values.push_back(new InfluenceMapFloat(resolution, size, coef, level, valueThresholdDebug));
	}
}

InfluenceMapCombine::~InfluenceMapCombine() {
	clear_vector(values);
}

void InfluenceMapCombine::update(Urho3D::Vector3& pos, float value) {

}

void InfluenceMapCombine::update(Physical* thing, float value) {
	auto& pos = thing->getPosition();
	const auto centerX = calculator.getIndex(pos.x_);
	const auto centerZ = calculator.getIndex(pos.z_);
	const auto minI = calculator.getValid(centerX - level);
	const auto maxI = calculator.getValid(centerX + level);

	const auto minJ = calculator.getValid(centerZ - level);
	const auto maxJ = calculator.getValid(centerZ + level);
	auto playerIDd= thing->getPlayer();
	
	for (short i = minI; i <= maxI; ++i) {
		const auto a = (i - centerX) * (i - centerX);
		for (short j = minJ; j <= maxJ; ++j) {
			const auto b = (j - centerZ) * (j - centerZ);
			int index = calculator.getNotSafeIndex(i, j);
			auto val = 1 / ((a + b) * coef + 1.f);
			for (int k = 0; k < values.size(); ++k) {
				//toDO bug player id??
				if (k == playerIDd) {
					//TOOD BUG wizac tylko wrogów a nie reszte
					values[k]->add(index, val);
				} else {
					values[k]->add(index, -val);
				}
			}
		}
	}
}

void InfluenceMapCombine::reset() {
	for (auto value : values) {
		value->reset();
	}
}

float InfluenceMapCombine::getValueAt(int index) const {
	return 0;
}

void InfluenceMapCombine::finishCalc() {
	for (auto value : values) {
		value->finishCalc();
	}
}

float InfluenceMapCombine::getValueAsPercent(const Urho3D::Vector2& pos) const {
	return 0.5;
}
