#pragma once
#include "InfluenceMapType.h"
#include "map/InfluenceMapFloat.h"
#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapI.h"
#include "map/InfluenceMapInt.h"

inline InfluenceMapI* createMap(short resolution, InfluenceMapType type, float size,
                                float coef, char level, float valueThresholdDebug) {
	switch (type) {
	case InfluenceMapType::INT:
		return new InfluenceMapInt(resolution, size, valueThresholdDebug);
	case InfluenceMapType::FLOAT:
		return new InfluenceMapFloat(resolution, size, coef, level, valueThresholdDebug);
	case InfluenceMapType::HISTORY:
		return new InfluenceMapHistory(resolution, size, coef, level, 0.0001f, 0.5f, valueThresholdDebug);
	default:
		assert(false);
		return nullptr;
	}
}
