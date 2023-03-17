#pragma once
#include <chrono>

#include "utils/defines.h"
constexpr bool FORCE_BENCHMARK_MODE = false;
constexpr bool FORCE_TRAIN_MODE = false;
constexpr bool FORCE_AI_OUTPUT = false;
constexpr bool FORCE_ALL_PLAYER_AI = false;
constexpr bool FORCE_NO_PLAYER_AI = false;
constexpr bool FORCE_HEADLESS = false;

constexpr float UPDATE_DRAW_DISTANCE = 120.f;

constexpr float TIME_PER_UPDATE = 0.033333333f;
constexpr unsigned char FRAMES_IN_PERIOD = 1 / TIME_PER_UPDATE;


inline struct SimGlobals {
	inline static bool BENCHMARK_MODE = FORCE_BENCHMARK_MODE;
	inline static bool TRAIN_MODE = FORCE_TRAIN_MODE;

	inline static bool ALL_PLAYER_AI = FORCE_ALL_PLAYER_AI;
	inline static bool NO_PLAYER_AI = FORCE_NO_PLAYER_AI;
	inline static bool HEADLESS = false;
	inline static bool FAKE_TERRAIN = false;
	inline static bool RANDOM = false;
	
	inline static Urho3D::String DATABASE_NUMBER = "";
	inline static std::string ACTION_AI_PATH[MAX_PLAYERS] = {"", ""};
	inline static std::string ORDER_AI_PATH[MAX_PLAYERS] = {"", ""};

	inline static Urho3D::Vector<Urho3D::String> OUTPUT_NAMES;

	inline static int CURRENT_RUN = 0;
	inline static int MAX_RUNS = 0;
	inline static std::chrono::system_clock::time_point SUPER_START = std::chrono::system_clock::now();

} SIM_GLOBALS;
