#pragma once
#include <array>
#include <magic_enum.hpp>

#include "SimGlobals.h"
#include "utils/OtherUtils.h"

constexpr unsigned int SECONDS_IN_MONTH = 5;

enum class PerFrameAction:char {
	INFLUENCE_UNITS = 0,
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

struct FrameInfo {
	void resetRealFrame() { realFrame = false; }

	void countFrame() {
		realFrame = true;
		accumulateTime -= TIME_PER_UPDATE;
		++totalTicks;
		++currentFrame;
		if (currentFrame >= FRAMES_IN_PERIOD) {
			currentFrame = 0;
			++seconds;
		}
	}

	void set(unsigned char frame, unsigned int secs) {
		currentFrame = frame;
		seconds = secs;
		realFrame = true;
	}

	unsigned char getCurrentFrame() const { return currentFrame; }
	unsigned char getLastProcessedFrame() const {
		return (currentFrame - 1 + FRAMES_IN_PERIOD) % FRAMES_IN_PERIOD;
	}
	bool isRealFrame() const { return realFrame; }
	unsigned int getSeconds() const { return seconds; }
	unsigned int getTotalTicks() const { return totalTicks; }

	std::pair<unsigned char, unsigned short> getDate() const {
		const auto allMonths = seconds / SECONDS_IN_MONTH;
		return {allMonths % 12, allMonths / 12};
	}

	std::tuple<unsigned char, unsigned char, unsigned char> getTime() const {
		return std::make_tuple(seconds / 3600, (seconds / 60) % 60, seconds % 60);
	}

	bool shouldRun(PerFrameAction type) const {
		return get(type, currentFrame, seconds);
	}

	bool canUpdate(PerFrameAction type, bool force) const {
		return force || shouldRun(type);
	}

	void reset() {
		seconds = 0;
		totalTicks = 0;
		currentFrame = 0;
		realFrame = false;
		wallTime = 0;
		accumulateTime = 0;
	}

	static bool get(PerFrameAction type, unsigned char frameNum, unsigned int second = 0) {
		const auto val = data[castC(type)][frameNum];
		return val != 0 && (second+1) % val == 0;
	}

	void addWallTime(float time) { wallTime += time; }
	float getWallTime() const { return wallTime; }

	float accumulate(float timeStep) {
		accumulateTime += SIM_GLOBALS.BENCHMARK_MODE ? TIME_PER_UPDATE : timeStep;
		return accumulateTime;
	}

	float getAccumulateTime() const { return accumulateTime; }

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
		}, //INFLUENCE_UNITS
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

	unsigned int seconds = 0;
	unsigned int totalTicks = 0;
	unsigned char currentFrame = 0;
	bool realFrame = false;
	float wallTime = 0;
	float accumulateTime = 0;
};
