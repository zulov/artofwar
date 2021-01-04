#pragma once
constexpr bool FORCE_BENCHMARK_MODE = true;
constexpr bool FORCE_TRAIN_MODE = false;
constexpr float UPDATE_DRAW_DISTANCE = 120.f;

constexpr float TIME_PER_UPDATE = 0.033333333f;
constexpr unsigned char FRAMES_IN_PERIOD = 1 / TIME_PER_UPDATE;


inline struct SimGlobals {
	inline static bool BENCHMARK_MODE = FORCE_BENCHMARK_MODE;
	inline static bool TRAIN_MODE = FORCE_TRAIN_MODE;
private:

} SIM_GLOBALS;
