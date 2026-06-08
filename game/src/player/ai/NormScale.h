#pragma once

namespace NormScale {
	inline constexpr float SCORE       = 1000.f; // player/enemy score (and score deltas)
	inline constexpr float ARMY        = 200.f;  // army / free-army / enemy-army unit counts
	inline constexpr float WORKERS     = 100.f;  // worker / free-worker counts
	inline constexpr float BUILDINGS   = 50.f;   // total building count
	inline constexpr float ATTACK      = 1000.f; // unit attack sum (army offense)
	inline constexpr float DEFENCE     = 100.f;  // building attack sum (static defence)
	inline constexpr float RES         = 1000.f; // resource stockpile amount (and res delta)
	inline constexpr float GATHER      = 10.f;   // resource gather speed
	inline constexpr float GATHER_DELTA = 20.f;  // per-tick change in total gather speed
	inline constexpr float GAME_TIME   = 1800.f; // game time in seconds (~30 min horizon)
	inline constexpr float GAME_TIME_SHORT = 600.f; // shorter game-time horizon (~10 min) for economy/unit brains

	inline constexpr float TOTAL_LACKING   = 2000.f; // summed resource deficit across all wants
	inline constexpr float LACKING_PER_RES = 500.f;  // per-resource deficit
	inline constexpr float REFINE_GAP      = 10.f;   // gather speed beyond refine capacity
	inline constexpr float NEARBY_SUPPLY   = 5000.f; // pooled HP of nearby food/wood nodes
	inline constexpr float RES_BUILDINGS   = 20.f;   // resource-building counts / res-without-bonus sum

	// History scores (time-weighted recency over a lookback window).
	inline constexpr float ACTIVITY    = 10.f;   // attack/defend activity scores
	inline constexpr float FAILURE     = 5.f;    // attack/defend/unit failure scores
	inline constexpr float BUILD_FAILURE   = 10.f; // building/collect failure scores
}
