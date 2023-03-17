#pragma once

struct ChargeData {
	float energy;
	float energyMax;
	float attackRange;

	ChargeData(float energyMax, float attackRange)
		: energyMax(energyMax),
		  attackRange(attackRange) {
		reset();
	}

	ChargeData(const ChargeData&) = delete;

	void reset() {
		energy = energyMax;
	}

	bool updateFrame() {
		--energy;
		return energy > 0;
	}

	bool updateHit(float before, float after) {
		if (after < 0.1f && before - after > 0.1f) {
			++energy;
		} else {
			energy -= 5;
		}
		return energy > 0;
	}
};
