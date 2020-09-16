#include "InfluenceMapCombine.h"

#include <span>
#include "Game.h"
#include "objects/Physical.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/DeleteUtils.h"

InfluenceMapCombine::InfluenceMapCombine(unsigned short resolution, float size, float coef, char level,
                                         float valueThresholdDebug, char numberOfMaps) :
	calculator(GridCalculatorProvider::get(resolution, size)), coef(coef), level(level), numberOfMaps(numberOfMaps) {
	for (int i = 0; i < numberOfMaps; ++i) {
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

void InfluenceMapCombine::update(Physical* thing, std::span<float> values1) {
	assert(values1.size()==values.size());
	auto& pos = thing->getPosition();
	const auto centerX = calculator->getIndex(pos.x_);
	const auto centerZ = calculator->getIndex(pos.z_);
	if (centerX != prevCenterX || centerZ != prevCenterZ) {
		prevCenterX = centerX;
		prevCenterZ = centerZ;

		const auto minI = calculator->getValid(centerX - level);
		const auto maxI = calculator->getValid(centerX + level);

		const auto minJ = calculator->getValid(centerZ - level);
		const auto maxJ = calculator->getValid(centerZ + level);

		k = 0;
		for (short i = minI; i <= maxI; ++i) {
			const auto a = (i - centerX) * (i - centerX);
			for (short j = minJ; j <= maxJ; ++j) {
				const auto b = (j - centerZ) * (j - centerZ);

				vals[k] = 1 / ((a + b) * coef + 1.f);
				indexes[k] = calculator->getNotSafeIndex(i, j);
				++k;
			}
		}
		--k;
	}

	for (int i = 0; i < values.size(); ++i) {
		values[i]->add(indexes, vals, k, values1[i]);
	}

}

void InfluenceMapCombine::reset() {
	for (auto value : values) {
		value->reset();
	}
}

void InfluenceMapCombine::finishCalc() {
	for (auto value : values) {
		value->finishCalc();
	}
}

InfluenceMapFloat* InfluenceMapCombine::get(char index) {
	assert(index<numberOfMaps);
	return values[index];
}

void InfluenceMapCombine::print(Urho3D::String name) {
	for (int i = 0; i < numberOfMaps; ++i) {
		values[i]->print(name + Urho3D::String(i));
	}
}
