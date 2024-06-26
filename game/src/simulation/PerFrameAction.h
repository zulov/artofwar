#pragma once
#include <array>
#include <magic_enum.hpp>

#include "FrameInfo.h"
#include "SimGlobals.h"
#include "utils/OtherUtils.h"

enum class PerFrameAction:char {
	INFLUENCE_UNITS_1 = 0,
	INFLUENCE_UNITS_2,
	INFLUENCE_RESOURCES,
	INFLUENCE_OTHER,
	INFLUENCE_QUAD_OTHER,
	SELF_AI,
	AI_ACTION,
	AI_ORDER,
	QUEUE_HUD,
	HUD_UPDATE,
	VISIBILITY,
	INFLUENCE_HISTORY_RESET,
	RESOURCE_GATHER_SPEED,
	RESOURCE_MONTH_UPDATE,
	RESOURCE_YEAR_UPDATE,
};

constexpr inline struct PerFrameActionData {
private:
	inline static const std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_SECOND = {
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	inline static const std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_2_SECONDS = {
		2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	inline static const std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_MONTH = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, SECONDS_IN_MONTH
	};
	inline static const std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_YEAR = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, SECONDS_IN_MONTH * 12
	};
	inline static const std::array<unsigned char, FRAMES_IN_PERIOD> data[magic_enum::enum_count<PerFrameAction>()] = {
		{
			2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_UNITS_1
		{
			0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_UNITS_2
		{
			0, 0, 10, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_RESOURCES		
		{
			0, 0, 0, 5, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_OTHER
		{
			0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_QUAD_OTHER
		{
			0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
			0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
			0, 0, 0, 1, 0, 0, 0, 0, 0, 1
		}, //SELF_AI
		{
			0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //AI_ACTION
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //AI_ORDER
		ONCE_PER_2_SECONDS, //QUEUE_HUD
		ONCE_PER_SECOND, //HUD_UPDATE
		{
			0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //VISIBILITY
		{
			0, 0, 0, 30, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_HISTORY_RESET
		ONCE_PER_SECOND, //RESOURCE_GATHER_SPEED
		ONCE_PER_MONTH, //RESOURCE_MONTH_UPDATE
		ONCE_PER_YEAR //RESOURCE_YEAR_UPDATE
	};

public:
	static bool get(PerFrameAction type, unsigned char frameNum, unsigned int second = 0) {
		const auto val = data[cast(type)][frameNum];
		return val != 0 && (second+1) % val == 0;
	}

	static bool get(PerFrameAction type, const FrameInfo* frameInfo) {
		return get(type, frameInfo->getCurrentFrame(), frameInfo->getSeconds());
	}
} PER_FRAME_ACTION;
