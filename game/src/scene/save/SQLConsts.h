#pragma once
#include <string>

struct SQLConsts {

#define BASIC_COL                                                                                                      \
	"id_db INT NOT NULL,"                                                                                              \
	"hp_coef INT NOT NULL,"                                                                                            \
	"uid INT PRIMARY KEY,"

#define PHYSICAL_COL                                                                                                   \
	BASIC_COL                                                                                                          \
	"player INT NOT NULL,"                                                                                             \
	"level INT NOT NULL,"

#define STATIC_COL                                                                                                     \
	"bucket_x INT NOT NULL,"                                                                                           \
	"bucket_y INT NOT NULL,"                                                                                           \
	"state INT NOT NULL,"                                                                                              \
	"next_state INT NOT NULL"

	inline static constexpr const char* BUILDING_NAME = "buildings";
	inline static constexpr const char* BUILDING_COL = "(" PHYSICAL_COL STATIC_COL ","
																			   "target_uid INT NOT NULL, "
															   "frame_state INT NOT NULL) WITHOUT ROWID;";

	inline static constexpr const char* RESOURCE_NAME = "resources";
	inline static constexpr const char* RESOURCE_COL = "(" BASIC_COL STATIC_COL ") WITHOUT ROWID;";

	inline static constexpr const char* UNIT_NAME = "units";
	inline static constexpr const char* UNIT_COL = "(" PHYSICAL_COL "position_x INT NOT NULL,"
																												   "position_z INT NOT NULL,"
																												   "state INT NOT NULL,"
																												   "velocity_x INT NOT NULL,"
																												   "velocity_z INT NOT NULL,"
																												   "next_state INT NOT NULL, state_pending INT NOT NULL,"
																												   "frame_state INT NOT NULL, formation INT NOT NULL, pos_in_state INT NOT NULL,"
																   "command_priority REAL NOT NULL, command_center INT NOT NULL,"
											   "charge_energy REAL NOT NULL,"
																   "target_uid INT NOT NULL, pending_target_uid INT NOT NULL,"
																   "aim_kind INT NOT NULL, aim_target_uid INT NOT NULL, aim_current INT NOT NULL,"
																   "aim_direction_x REAL NOT NULL, aim_direction_z REAL NOT NULL,"
																   "pending_aim_kind INT NOT NULL, pending_aim_target_uid INT NOT NULL,"
																   "pending_aim_current INT NOT NULL, pending_aim_direction_x REAL NOT NULL,"
																   "pending_aim_direction_z REAL NOT NULL) WITHOUT ROWID;";

	inline static constexpr const char* PLAYER_NAME = "players";
	inline static constexpr const char* PLAYER_COL = "(id INT PRIMARY KEY,"
													 "is_active INT NOT NULL,"
													 "team INT NOT NULL,"
													 "nation INT NOT NULL,"
													 "buildingUid INT NOT NULL,"
													 "unitUid INT NOT NULL,"
																 "food INT NOT NULL,"
																 "wood INT NOT NULL,"
																 "stone INT NOT NULL,"
																 "gold INT NOT NULL,"
																 "gather_food REAL NOT NULL, gather_wood REAL NOT NULL,"
																 "gather_stone REAL NOT NULL, gather_gold REAL NOT NULL,"
																										 "pending_gather_food REAL NOT NULL, pending_gather_wood REAL NOT NULL,"
																										 "pending_gather_stone REAL NOT NULL, pending_gather_gold REAL NOT NULL,"
																										 "sum_food REAL NOT NULL, sum_wood REAL NOT NULL,"
															 "sum_stone REAL NOT NULL, sum_gold REAL NOT NULL) WITHOUT ROWID;";

	inline static constexpr const char* CONFIG_NAME = "config";
	inline static constexpr const char* CONFIG_COL = "(precision INT NOT NULL,"
															 "map INT NOT NULL,"
															 "size INT NOT NULL, total_ticks INT NOT NULL,"
																		 "random_present INT NOT NULL, random_seed INT NOT NULL,"
																		 "random_float_ai_index INT NOT NULL,"
																		 "random_float_resource_rotation_index INT NOT NULL,"
																		 "random_float_collision_force_index INT NOT NULL,"
																		 "random_float_other_index INT NOT NULL,"
																		 "random_int_save_index INT NOT NULL,"
																		 "random_int_player_name_index INT NOT NULL,"
																									 "random_int_resource_node_index INT NOT NULL)";

	inline static constexpr const char* UNIT_ORDER_NAME = "unit_orders";
	inline static constexpr const char* UNIT_ORDER_COL = "(unit_uid INT NOT NULL, order_idx INT NOT NULL, action INT "
														 "NOT NULL, append INT NOT NULL, has_target INT NOT NULL, "
														 "target_uid INT NOT NULL, x REAL NOT NULL, z REAL NOT NULL, "
														 "PRIMARY KEY(unit_uid, order_idx)) WITHOUT ROWID;";
	inline static constexpr const char* AIM_PATH_NAME = "aim_paths";
	inline static constexpr const char* AIM_PATH_COL =
			"(unit_uid INT NOT NULL, pending INT NOT NULL, order_idx INT NOT NULL, cell INT NOT NULL, "
			"PRIMARY KEY(unit_uid, pending, order_idx)) WITHOUT ROWID;";
	inline static constexpr const char* QUEUE_NAME = "queues";
	inline static constexpr const char* QUEUE_COL = "(owner_id INT NOT NULL, owner_type INT NOT NULL, order_idx INT "
													"NOT NULL, type INT NOT NULL, id INT NOT NULL, "
																												 "level_id INT NOT NULL, amount INT NOT NULL, elapsed_ticks INT NOT NULL, "
													"PRIMARY KEY(owner_id, owner_type, order_idx)) WITHOUT ROWID;";
	inline static constexpr const char* PLAYER_LEVEL_NAME = "player_levels";
	inline static constexpr const char* PLAYER_LEVEL_COL =
			"(player INT NOT NULL, type INT NOT NULL, id INT NOT NULL, level INT NOT NULL, PRIMARY KEY(player, type, "
			"id)) WITHOUT ROWID;";
	inline static constexpr const char* AI_STATE_NAME = "ai_state";
	inline static constexpr const char* AI_STATE_COL =
			"(player INT PRIMARY KEY, prev_score INT NOT NULL, prev_enemy_score INT NOT NULL, prev_units INT NOT NULL, "
			"prev_res_sum REAL NOT NULL, prev_gather_sum REAL NOT NULL, food_priority REAL NOT NULL, "
			"wood_priority REAL NOT NULL, stone_priority REAL NOT NULL, gold_priority REAL NOT NULL, "
			"pressure_our_army_our_econ REAL NOT NULL, pressure_our_army_our_building REAL NOT NULL, "
			"pressure_our_army_enemy_army REAL NOT NULL, pressure_our_army_enemy_econ REAL NOT NULL, "
			"pressure_our_army_enemy_building REAL NOT NULL, pressure_our_army_battle REAL NOT NULL, "
			"pressure_our_econ_our_building REAL NOT NULL, pressure_our_econ_enemy_army REAL NOT NULL, "
			"pressure_our_econ_enemy_econ REAL NOT NULL, pressure_our_econ_enemy_building REAL NOT NULL, "
			"pressure_our_econ_battle REAL NOT NULL, pressure_our_building_enemy_army REAL NOT NULL, "
			"pressure_our_building_enemy_econ REAL NOT NULL, pressure_our_building_enemy_building REAL NOT NULL, "
			"pressure_our_building_battle REAL NOT NULL, pressure_enemy_army_enemy_econ REAL NOT NULL, "
			"pressure_enemy_army_enemy_building REAL NOT NULL, pressure_enemy_army_battle REAL NOT NULL, "
			"pressure_enemy_econ_enemy_building REAL NOT NULL, pressure_enemy_econ_battle REAL NOT NULL, "
			"pressure_enemy_building_battle REAL NOT NULL, lacking_food REAL NOT NULL, lacking_wood REAL NOT NULL, "
			"lacking_stone REAL NOT NULL, lacking_gold REAL NOT NULL) WITHOUT ROWID;";
	inline static constexpr const char* AI_WANT_NAME = "ai_wants";
	inline static constexpr const char* AI_WANT_COL =
			"(player INT NOT NULL, order_idx INT NOT NULL, priority REAL NOT NULL, base_priority REAL NOT NULL, "
			"type INT NOT NULL, count INT NOT NULL, specific_id INT NOT NULL, age INT NOT NULL, reserve_ticks INT NOT NULL, "
			"active INT NOT NULL, PRIMARY KEY(player, order_idx)) WITHOUT ROWID;";
	inline static constexpr const char* AI_HISTORY_NAME = "ai_history";
	inline static constexpr const char* AI_HISTORY_COL =
			"(player INT NOT NULL, action INT NOT NULL, order_idx INT NOT NULL, tick INT NOT NULL, type INT NOT NULL, "
			"result INT NOT NULL, "
			"chosen_id INT NOT NULL, PRIMARY KEY(player, action, order_idx)) WITHOUT ROWID;";
	inline static constexpr const char* PROJECTILE_NAME = "projectiles";
	inline static constexpr const char* PROJECTILE_COL =
			"(aim_uid INT NOT NULL, percent_to_go REAL NOT NULL, speed REAL NOT NULL, "
			"attack_val REAL NOT NULL, player INT NOT NULL);";
	inline static constexpr const char* FORMATION_NAME = "formations";
	inline static constexpr const char* FORMATION_COL =
			"(id INT PRIMARY KEY, state INT NOT NULL, type INT NOT NULL, direction_x REAL NOT NULL, "
			"direction_z REAL NOT NULL) WITHOUT ROWID;";
	inline static constexpr const char* FORMATION_ORDER_NAME = "formation_orders";
	inline static constexpr const char* FORMATION_ORDER_COL =
			"(formation_id INT NOT NULL, order_idx INT NOT NULL, pending INT NOT NULL, action INT NOT NULL, "
			"append INT NOT NULL, has_target INT NOT NULL, target_uid INT NOT NULL, x REAL NOT NULL, z REAL NOT NULL, "
			"PRIMARY KEY(formation_id, order_idx, pending)) WITHOUT ROWID;";
	inline static constexpr const char* PENDING_COMMAND_NAME = "pending_commands";
	inline static constexpr const char* PENDING_COMMAND_COL =
			"(order_idx INT PRIMARY KEY, kind INT NOT NULL, action INT NOT NULL, action_type INT NOT NULL, id_db INT "
			"NOT NULL, "
			"player INT NOT NULL, level INT NOT NULL, number INT NOT NULL, x REAL NOT NULL, z REAL NOT NULL, hp REAL "
			"NOT NULL, "
			"target_uid INT NOT NULL, formation_id INT NOT NULL, append INT NOT NULL) WITHOUT ROWID;";
	inline static constexpr const char* PENDING_COMMAND_ENTITY_NAME = "pending_command_entities";
	inline static constexpr const char* PENDING_COMMAND_ENTITY_COL =
			"(command_idx INT NOT NULL, order_idx INT NOT NULL, uid INT NOT NULL, PRIMARY KEY(command_idx, order_idx)) "
			"WITHOUT ROWID;";

	inline const static std::string CREATE_TABLE = "CREATE TABLE ";

	inline const static std::string SELECT = "SELECT * from ";
	inline const static std::string COUNT = "SELECT count(*) from ";
};
