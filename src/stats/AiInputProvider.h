#pragma once
#include <magic_enum.hpp>
#include <span>
#include "StatsEnums.h"
#include "database/db_strcut.h"

struct db_basic_metric;

class AiInputProvider {
public:

	AiInputProvider();
	
	std::span<float> getBasicInput(short id);

	std::span<float> getResourceInput(char playerId);
	std::span<float> getBasicInputWithMetric(char playerId, const db_basic_metric* prop);
private:

	void update(short id, float* data);

	float* data;
	std::span<float> dataSpan;

	float basicInput[magic_enum::enum_count<BasicInputType>() * 2];
	std::span<float> basicInputSpan;

	float resourceIdInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<ResourceInputType>()];
	std::span<float> resourceIdInputSpan;

	float basicWithParams[magic_enum::enum_count<BasicInputType>() * 2 + AI_PROPS_SIZE];
	std::span<float> basicWithParamsSpan;


	float wBasic[magic_enum::enum_count<BasicInputType>()];
	float wResourceInput[magic_enum::enum_count<ResourceInputType>()];
};
