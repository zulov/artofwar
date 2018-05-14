#pragma once
#include <Urho3D/Math/Vector2.h>
#include <iostream>

#define FORCE_STATS_SIZE 1000
#define FORCE_STATS_ENABLE false

struct ForceStats
{
	void reset() {
		sepObstStat = 0;
		sepUnitStat = 0;
		destStat = 0;
		formStat = 0;
		escaStat = 0;
	}

	ForceStats() {
		reset();
		std::fill_n(stats, 5, 0);
	}

	void addSepObst(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) {
			sepObstStat += force.Length();
		}
	}

	void addSepUnit(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { sepUnitStat += force.Length(); }
	}

	void addDest(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { destStat += force.Length(); }
	}

	void addForm(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { formStat += force.Length(); }
	}

	void addEscp(Urho3D::Vector2& force) {
		if constexpr (FORCE_STATS_ENABLE) { escaStat += force.Length(); }
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
		return stats;
	}

	float sepObstStat;
	float sepUnitStat;
	float destStat;
	float formStat;
	float escaStat;
	float stats[5];

	short statIndex = 0;
};
