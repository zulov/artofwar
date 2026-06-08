#pragma once
enum class CenterType:char {
	ECON=0,
	BUILDING,
	ARMY
};

// InfluenceManager builds mapsForCentersPerPlayer as {econ, buildings, army} and reads it
// with [castC(CenterType)]; getCenterOf therefore depends on this exact 0,1,2 order.
static_assert(static_cast<int>(CenterType::ECON) == 0
              && static_cast<int>(CenterType::BUILDING) == 1
              && static_cast<int>(CenterType::ARMY) == 2,
              "CenterType must stay ECON,BUILDING,ARMY == 0,1,2 (mapsForCentersPerPlayer is built in this order)");
