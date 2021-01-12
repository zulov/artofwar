#pragma once
#include "utils/defines.h"
constexpr bool FORCE_BENCHMARK_MODE = true;
constexpr bool FORCE_TRAIN_MODE = false;

constexpr float UPDATE_DRAW_DISTANCE = 120.f;

constexpr float TIME_PER_UPDATE = 0.033333333f;
constexpr unsigned char FRAMES_IN_PERIOD = 1 / TIME_PER_UPDATE;


inline struct SimGlobals {
	inline static bool BENCHMARK_MODE = FORCE_BENCHMARK_MODE;
	inline static bool TRAIN_MODE = FORCE_TRAIN_MODE;
	inline static Urho3D::String ACTION_AI_PATH[MAX_PLAYERS] = {Urho3D::String::EMPTY, Urho3D::String::EMPTY};
	inline static Urho3D::String ORDER_AI_PATH[MAX_PLAYERS] = {Urho3D::String::EMPTY, Urho3D::String::EMPTY};
	inline static Urho3D::Vector2 CAMERA_START = Urho3D::Vector2::ZERO;
private:

} SIM_GLOBALS;
