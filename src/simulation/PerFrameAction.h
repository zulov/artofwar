#pragma once
#include <array>
#include <magic_enum.hpp>

#include "FrameInfo.h"
#include "SimGlobals.h"
#include "utils/OtherUtils.h"

enum class PerFrameAction:char {
	INFLUENCE_UNITS_1=0,
	INFLUENCE_UNITS_2,
	INFLUENCE_RESOURCES,
	INFLUENCE_OTHER,
	SELF_AI,
	STAT_SAVE,
	AI_ACTION,
	AI_ORDER,
	QUEUE_HUD,
	HUD_UPDATE
};

inline struct PerFrameActionData {
private:
	inline static std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_SECOND = {
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	inline static std::array<unsigned char, FRAMES_IN_PERIOD> ONCE_PER_2_SECONDS = {
		2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	inline static std::array<unsigned char, FRAMES_IN_PERIOD> data[magic_enum::enum_count<PerFrameAction>()] = {
		{
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_UNITS_1
		{
			0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_UNITS_2
		{
			0, 0, 10, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_RESOURCES		
		{
			0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //INFLUENCE_OTHER
		{
			0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
			0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
			0, 0, 0, 1, 0, 0, 0, 0, 1, 0
		}, //SELF_AI
		{
			10, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, //STAT_SAVE
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
		ONCE_PER_SECOND //HUD_UPDATE
	};
public:
	static bool get(PerFrameAction type, unsigned char frameNum, unsigned int second = 1) {
		auto val = data[cast(type)][frameNum];
		return val != 0 && second % val == 0;
	}

	static bool get(PerFrameAction type, const FrameInfo* frameInfo) {
		auto val = data[cast(type)][frameInfo->getCurrentFrame()];
		return val != 0 && frameInfo->getSeconds() % val == 0;
	}
} PER_FRAME_ACTION;
