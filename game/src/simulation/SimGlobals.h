#pragma once
#include <chrono>
#include <string>
#include <vector>

constexpr short UNITS_NUMBER = 100;
constexpr char MAX_PLAYERS = 2;
constexpr bool DEBUG_LINES_ENABLED = true;

constexpr bool FORCE_BENCHMARK_MODE = false;
constexpr bool FORCE_ALL_PLAYER_AI = false;
constexpr bool FORCE_NO_PLAYER_AI = false;

constexpr float UPDATE_DRAW_DISTANCE = 120.f;

constexpr float TIME_PER_UPDATE = 0.033333333f;
constexpr unsigned char FRAMES_IN_PERIOD = 1 / TIME_PER_UPDATE;


inline struct SimGlobals {
	inline static bool BENCHMARK_MODE = FORCE_BENCHMARK_MODE;
	inline static unsigned char FRAME_PACK = 1;

	inline static bool ALL_PLAYER_AI = FORCE_ALL_PLAYER_AI;
	inline static bool NO_PLAYER_AI = FORCE_NO_PLAYER_AI;
	inline static bool HEADLESS = false;
	inline static bool FAKE_TERRAIN = false;
	inline static bool RANDOM = false;

	inline static std::string BRAIN_AI_PATH[MAX_PLAYERS] = {"", ""};

	inline static std::vector<std::string> OUTPUT_NAMES;

	inline static int CURRENT_RUN = 0;
	inline static int MAX_RUNS = 0;
	inline static std::chrono::system_clock::time_point SUPER_START = std::chrono::system_clock::now();

} SIM_GLOBALS;
