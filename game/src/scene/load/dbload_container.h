#pragma once
#include <Urho3D/Container/Str.h>
#include <optional>
#include <unordered_map>
#include <vector>

#include "RuntimeSaveData.h"
#include "database/db_columns.h"
#include "database/db_utils.h"
#include "utils/DeleteUtils.h"

struct dbload_config {
	dbload_config(int precision, int map, int size, unsigned totalTicks) :
		precision(precision), map(map), size(size), frame{totalTicks} {}

	int precision;
	int map;
	int size;
	FrameSaveData frame;
};

struct dbload_physical {
	float hp;
	unsigned uid;
	unsigned short id_db;
	char player;
	char level;
	char state;
	char nextState;

	dbload_physical(unsigned short idDb, float hp, unsigned uid, char player, char level, char state, char nextState) :
		hp(hp), uid(uid), id_db(idDb), player(player), level(level), state(state), nextState(nextState) {}
};

struct dbload_static : dbload_physical {
	unsigned short buc_x;
	unsigned short buc_y;

	dbload_static(unsigned short idDb, float hp, unsigned uid, char player, unsigned short bucX, unsigned short bucY,
				  char level, char state, char nextState) :
		dbload_physical(idDb, hp, uid, player, level, state, nextState), buc_x(bucX), buc_y(bucY) {}
};

struct dbload_unit : dbload_physical {
	float pos_x;
	float pos_z;
	float vel_x;
	float vel_z;
	UnitRuntimeSaveData runtime;

	using C = UnitCol;

	dbload_unit(sqlite3_stmt* stmt, int p) :
		dbload_physical(asUShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p), asUI(stmt, C::uid), asByte(stmt, C::player),
					asByte(stmt, C::level), asByte(stmt, C::state), -1),
			pos_x(asItoF(stmt, C::position_x, p)), pos_z(asItoF(stmt, C::position_z, p)),
			vel_x(asItoF(stmt, C::velocity_x, p)), vel_z(asItoF(stmt, C::velocity_z, p)) {
		runtime.uid = uid;
		runtime.targetUid = asUI(stmt, C::target_uid);
		runtime.pendingTargetUid = asUI(stmt, C::pending_target_uid);
		runtime.nextState = asByte(stmt, C::next_state);
		runtime.stateChangePending = asBool(stmt, C::state_pending);
		runtime.currentFrameState = asUShort(stmt, C::frame_state);
		runtime.formation = asShort(stmt, C::formation);
		runtime.posInState = asShort(stmt, C::pos_in_state);
		runtime.commandPriority = asScaledFloat(stmt, C::command_priority, p);
		runtime.commandCenter = asUByte(stmt, C::command_center);
		runtime.chargeEnergy = asScaledFloat(stmt, C::charge_energy, p);
	runtime.aim.kind = static_cast<AimSaveKind>(asByte(stmt, C::aim_kind));
	runtime.aim.targetUid = asUI(stmt, C::aim_target_uid);
	runtime.aim.current = asShort(stmt, C::aim_current);
	runtime.aim.directionX = asScaledFloat(stmt, C::aim_direction_x, p);
	runtime.aim.directionZ = asScaledFloat(stmt, C::aim_direction_z, p);
	runtime.pendingAim.kind = static_cast<AimSaveKind>(asByte(stmt, C::pending_aim_kind));
	runtime.pendingAim.targetUid = asUI(stmt, C::pending_aim_target_uid);
	runtime.pendingAim.current = asShort(stmt, C::pending_aim_current);
	runtime.pendingAim.directionX = asScaledFloat(stmt, C::pending_aim_direction_x, p);
	runtime.pendingAim.directionZ = asScaledFloat(stmt, C::pending_aim_direction_z, p);
	}

	dbload_unit(unsigned short idDb, float hp, unsigned uid, char player, char level, float posX, float posZ,
				char state, float velX, float velZ) :
		dbload_physical(idDb, hp, uid, player, level, state, -1), pos_x(posX), pos_z(posZ), vel_x(velX), vel_z(velZ) {}
};

struct dbload_building : dbload_static {
	unsigned thingToInteract;
	unsigned short currentFrameState;
	using C = BuildingCol;

	dbload_building(sqlite3_stmt* stmt, int p) :
		 dbload_building(asUShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p), asUI(stmt, C::uid),
						asByte(stmt, C::player), asByte(stmt, C::level), asUShort(stmt, C::bucket_x),
						asUShort(stmt, C::bucket_y), asByte(stmt, C::state), asByte(stmt, C::next_state),
						asUI(stmt, C::target_uid), asUShort(stmt, C::frame_state)) {}

	dbload_building(unsigned short idDb, float hpCoef, unsigned uid, char player, char level, unsigned short bucX,
					unsigned short bucY, char state, char nextState, unsigned targetUid,
					unsigned short frameState) :
		dbload_static(idDb, hpCoef, uid, player, bucX, bucY, level, state, nextState), thingToInteract(targetUid),
					currentFrameState(frameState) {}
};

struct dbload_resource : dbload_static {
	using C = ResourceCol;

	dbload_resource(sqlite3_stmt* stmt, int p) :
		dbload_resource(asUShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p), asUI(stmt, C::uid),
						asUShort(stmt, C::bucket_x), asUShort(stmt, C::bucket_y), asByte(stmt, C::state),
						asByte(stmt, C::next_state)) {}

	dbload_resource(unsigned short idDb, float hpCoef, unsigned uid, unsigned short bucX, unsigned short bucY,
					char state, char nextState) :
		dbload_static(idDb, hpCoef, uid, -1, bucX, bucY, -1, state, nextState) {}
};

struct dbload_player {
	bool is_active;
	unsigned char id;
	unsigned char team;
	unsigned char nation;
	unsigned buildingUid;
	unsigned unitUid;
	float food;
	float wood;
	float stone;
	float gold;
	ResourcesSaveData resources;

	using C = PlayerCol;

	dbload_player(sqlite3_stmt* stmt, int p) :
		dbload_player(asUByte(stmt, C::id), asBool(stmt, C::is_active), asUByte(stmt, C::team),
					  asUByte(stmt, C::nation),
					  asUI(stmt, C::buildingUid), asUI(stmt, C::unitUid), asItoF(stmt, C::food, p),
					  asItoF(stmt, C::wood, p), asItoF(stmt, C::stone, p), asItoF(stmt, C::gold, p)) {
		resources.player = id;
		resources.gatherSpeeds1s = {asScaledFloat(stmt, C::gather_food, p), asScaledFloat(stmt, C::gather_wood, p),
				asScaledFloat(stmt, C::gather_stone, p), asScaledFloat(stmt, C::gather_gold, p)};
		resources.sumGatherSpeed = {asScaledFloat(stmt, C::pending_gather_food, p),
				asScaledFloat(stmt, C::pending_gather_wood, p), asScaledFloat(stmt, C::pending_gather_stone, p),
				asScaledFloat(stmt, C::pending_gather_gold, p)};
		resources.sumValues = {asScaledFloat(stmt, C::sum_food, p), asScaledFloat(stmt, C::sum_wood, p),
				asScaledFloat(stmt, C::sum_stone, p), asScaledFloat(stmt, C::sum_gold, p)};
	}

	dbload_player(unsigned char id, bool isActive, unsigned char team, unsigned char nation, unsigned buildingUid,
				  unsigned unitUid, float food, float wood, float stone,
				  float gold) :
		is_active(isActive), id(id), team(team), nation(nation), buildingUid(buildingUid),
		unitUid(unitUid), food(food), wood(wood), stone(stone), gold(gold) {}
};

struct dbload_container {
	dbload_container() = default;

	~dbload_container() {
		delete config;
		clear_and_delete_vector(players);

		clear_and_delete_vector(units);
		clear_and_delete_vector(buildings);
		clear_and_delete_vector(resources);
	}

	dbload_config* config{};

	std::vector<dbload_player*>* players{};

	std::vector<dbload_unit*>* units{};
	std::vector<dbload_building*>* buildings{};
	std::vector<dbload_resource*>* resources{};

	std::unordered_map<unsigned, UnitVariableSaveData> unitVariable;
	std::vector<QueueSaveData> queues;
	std::vector<PlayerLevelSaveData> playerLevels;
	std::vector<ProjectileSaveData> projectiles;
	std::vector<FormationSaveData> formations;
	std::vector<FormationOrderRow> formationOrders;
	std::vector<PendingCommandSaveData> pendingCommands;
	std::vector<AiSaveData> aiStates;
	std::vector<AiWantSaveData> aiWants;
	std::vector<AiHistorySaveData> aiHistory;
	std::optional<FrameSaveData> frame;
	std::optional<RandSaveData> random;
};
