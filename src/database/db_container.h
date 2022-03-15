#pragma once

#include "db_strcut.h"
#include "player/ai/AiMetric.h"
#include "utils/DeleteUtils.h"

struct db_hud_vars;
struct db_map;
struct db_hud_size;
struct db_graph_settings;
struct db_resolution;
struct db_settings;
struct db_player_colors;

struct db_container {
	std::vector<db_hud_size*> hudSizes;
	std::vector<db_hud_vars*> hudVars;
	std::vector<db_graph_settings*> graphSettings;
	std::vector<db_resolution*> resolutions;
	std::vector<db_settings*> settings;

	std::vector<db_map*> maps;

	std::vector<db_unit*> units;
	std::vector<db_unit_level*> unitsLevels;
	std::vector<db_building*> buildings;
	std::vector<db_building_level*> buildingsLevels;
	std::vector<db_nation*> nations;
	std::vector<db_resource*> resources;
	std::vector<db_player_colors*> playerColors;
	std::vector<db_order*> orders;

	explicit db_container() = default;

	~db_container() {
		clear_vector(hudSizes);
		clear_vector(hudVars);
		clear_vector(resolutions);
		clear_vector(graphSettings);
		clear_vector(settings);

		clear_vector(maps);

		clear_vector(resources);
		clear_vector(nations);
		clear_vector(units);
		clear_vector(orders);
		clear_vector(buildings);
		clear_vector(playerColors);
		clear_vector(unitsLevels);
		clear_vector(buildingsLevels);
	}

	void finish() {
		for (auto* level : unitsLevels) {
			const auto unit = units[level->unit];
			level->finish(METRIC_DEFINITIONS.getUnitNorm(unit, level),
			              METRIC_DEFINITIONS.getUnitNormForSum(unit, level),
			              METRIC_DEFINITIONS.getUnitNormSmall(unit, level));
		}

		for (auto* level : buildingsLevels) {
			const auto building = buildings[level->building];
			level->finish(
				METRIC_DEFINITIONS.getBuildingNorm(building, level), METRIC_DEFINITIONS.getBuildingNormForSum(building, level),
				METRIC_DEFINITIONS.getBuildingOtherIdxs(), METRIC_DEFINITIONS.getBuildingDefenceIdxs(),
				METRIC_DEFINITIONS.getBuildingResourceIdxs(), METRIC_DEFINITIONS.getBuildingTechIdxs(),
				METRIC_DEFINITIONS.getBuildingUnitsIdxs(), METRIC_DEFINITIONS.getBuildingTypesIdxs());
		}
	}
};
