#pragma once
#include "utils/defines.h"
#include <Urho3D/Math/Vector2.h>
#include <iostream>

constexpr unsigned short FORCE_STATS_SIZE = 1000;
constexpr bool FORCE_STATS_ENABLE = false;

struct ForceStats {
	void vectorReset() {
		if constexpr (DEBUG_LINES_ENABLED) {
			sepObstLast = {};
			sepUnitLast = {};
			destLast = {};
			formLast = {};
			escaLast = {};
		}
	}

	void reset() {
		sepObstStat = 0;
		sepUnitStat = 0;
		destStat = 0;
		formStat = 0;
		escaStat = 0;
	}

	ForceStats() {
		reset();
		std::fill_n(stats, 5, 0.f);
	}

	ForceStats(const ForceStats&) = delete;

	void addSepObst(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) {
			sepObstStat += force.Length();
		}
		if constexpr (DEBUG_LINES_ENABLED) {
			sepObstLast = force;
			sepObstLast.Normalize();
		}
	}

	void addSepUnit(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { sepUnitStat += force.Length(); }
		if constexpr (DEBUG_LINES_ENABLED) {
			sepUnitLast = force;
			sepUnitLast.Normalize();
		}
	}

	void addDest(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { destStat += force.Length(); }
		if constexpr (DEBUG_LINES_ENABLED) {
			destLast = force;
			destLast.Normalize();
		}
	}

	void addForm(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { formStat += force.Length(); }
		if constexpr (DEBUG_LINES_ENABLED) {
			formLast = force;
			formLast.Normalize();
		}
	}

	void addEscp(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { escaStat += force.Length(); }
		if constexpr (DEBUG_LINES_ENABLED) {
			escaLast = force;
			escaLast.Normalize();
		}
	}

	float* result() {
		if constexpr (FORCE_STATS_ENABLE) {
			++statIndex;
			if (statIndex >= FORCE_STATS_SIZE) {
				stats[0] = sepObstStat / FORCE_STATS_SIZE;
				stats[1] = sepUnitStat / FORCE_STATS_SIZE;
				stats[2] = destStat / FORCE_STATS_SIZE;
				stats[3] = formStat / FORCE_STATS_SIZE;
				stats[4] = escaStat / FORCE_STATS_SIZE;
				statIndex = 0;
				std::cout <<
					"sepObs-" << stats[0] << "\tsepUnit-" << stats[1] << "\tdestStat-" <<
					stats[2] << "\tformStat-" << stats[3] << "\tescaStat-" <<
					stats[4] << std::endl;
				reset();
			}
		}
		longestLast = 0;
		return stats;
	}

	Urho3D::Vector2 sepObstLast;
	Urho3D::Vector2 sepUnitLast;
	Urho3D::Vector2 destLast;
	Urho3D::Vector2 formLast;
	Urho3D::Vector2 escaLast;

	float longestLast = 0;

	float sepObstStat;
	float sepUnitStat;
	float destStat;
	float formStat;
	float escaStat;

	float stats[5];

	short statIndex = 0;
};
