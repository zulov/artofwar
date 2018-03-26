#pragma once
#include <iostream>
#include <Urho3D/Math/Vector2.h>

#define FORCE_STATS_SIZE 10000

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
		sepObstStat += force.Length();
	}

	void addSepUnit(Urho3D::Vector2& force) {
		sepUnitStat += force.Length();
	}

	void addDest(Urho3D::Vector2& force) {
		destStat += force.Length();
	}

	void addForm(Urho3D::Vector2& force) {
		formStat += force.Length();
	}

	void addEscp(Urho3D::Vector2& force) {
		escaStat += force.Length();
	}

	float* result() {
		++statIndex;
		if (statIndex >= FORCE_STATS_SIZE) {
			stats[0] = sepObstStat / FORCE_STATS_SIZE;
			stats[1] = sepUnitStat / FORCE_STATS_SIZE;
			stats[2] = destStat / FORCE_STATS_SIZE;
			stats[3] = formStat / FORCE_STATS_SIZE;
			stats[4] = escaStat / FORCE_STATS_SIZE;
			statIndex = 0;
			std::cout <<
				stats[0] << "\t" << stats[1] << "\t" <<
				stats[2] << "\t" << stats[3] << "\t" <<
				stats[4] << std::endl;
			reset();
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
