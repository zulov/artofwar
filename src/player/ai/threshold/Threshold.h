#pragma once

#include <magic_enum.hpp>
#include <span>
#include "objects/Metrics.h"

enum class CenterType: char;

class Threshold {
public:
	Threshold(std::string filename);
	Threshold(const Threshold& rhs) = delete;
	
	bool ifAttack(std::span<float> value);
	CenterType getBestToAttack(std::span<float> value);
private:
	bool hasReach(float* threshold, std::span<float> value) const;
	float diff(float* threshold, std::span<float> value) const;

	float econAttackCenter[magic_enum::enum_count<UnitMetric>() - 1] = {1.f, 2.f, 3.f, 2.f, 2.f};
	float buildingAttackCenter[magic_enum::enum_count<UnitMetric>() - 1] = {2.f, 3.f, 2.f, 2.f, 1.f};
	float unitsAttackCenter[magic_enum::enum_count<UnitMetric>() - 1] = {3.f, 2.f, 2.f, 1.f, 2.f};

	//std::vector<float>

	std::string filename;
};
