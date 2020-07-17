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

	auto sizetemp = (level + level + 1) * (level + level + 1);
	indexes = new int[sizetemp];
	vals = new float[sizetemp];
}

InfluenceMapCombine::~InfluenceMapCombine() {
	clear_vector(values);

	delete [] indexes;
	delete [] vals;
}

void InfluenceMapCombine::update(Urho3D::Vector3& pos, float value) {}

void InfluenceMapCombine::update(Physical* thing, float value) {
	auto& pos = thing->getPosition();
	const auto centerX = calculator.getIndex(pos.x_);
	const auto centerZ = calculator.getIndex(pos.z_);
	if (centerX != prevCenterX || centerZ != prevCenterZ) {
		prevCenterX = centerX;
		prevCenterZ = centerZ;

		const auto minI = calculator.getValid(centerX - level);
		const auto maxI = calculator.getValid(centerX + level);

		const auto minJ = calculator.getValid(centerZ - level);
		const auto maxJ = calculator.getValid(centerZ + level);
		auto playerIDd = thing->getPlayer();
		char t[] = {1, -1, -1, 1};

		k = 0;
		for (short i = minI; i <= maxI; ++i) {
			const auto a = (i - centerX) * (i - centerX);
			for (short j = minJ; j <= maxJ; ++j) {
				const auto b = (j - centerZ) * (j - centerZ);
				int index = calculator.getNotSafeIndex(i, j);
				auto val = 1 / ((a + b) * coef + 1.f);
				//  for (int m = 0; m < values.size(); ++m) {
				// // 	//toDO bug player id??
				// 	if (m == playerIDd) {
				// 		//TOOD BUG wizac tylko wrogów a nie reszte
				// 		values[m]->add(index, val);
				// 	} else {
				// 		values[m]->add(index, -val);
				// 	}
				//  }
				//for (int i = 0; i < values.size(); ++i) {
				//values[i]->add(index, t[i] * val);
				//values[0]->add(index, t[0] * val);
				//values[1]->add(index, t[1] * val);
				//}
				vals[k] = val;
				indexes[k] = index;
				++k;
			}
		}
		--k;
	}

	for (auto influenceMapFloat : values) {
		for (int i = 0; i < k; ++i) {
			influenceMapFloat->add(indexes[i], vals[i]);
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
