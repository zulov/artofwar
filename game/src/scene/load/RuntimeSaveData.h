#pragma once

#include <array>
#include <vector>

#include "objects/resource/ResourceType.h"

struct QueueSaveData {
	unsigned ownerId{};
	unsigned char ownerType{};
	char type{};
	unsigned short id{};
	unsigned short levelId{};
	unsigned short amount{};
	unsigned short elapsedTicks{};
};

struct QueueRow {
	QueueSaveData data;
	unsigned short orderIndex{};
};

enum class AimSaveKind : char { NONE, TARGET, FOLLOW, CHARGE };

struct AimSaveData {
	AimSaveKind kind{};
	unsigned targetUid{};
	short current{};
	float directionX{};
	float directionZ{};
	std::vector<int> path;
};

struct UnitOrderSaveData {
	unsigned unitUid{};
	char action{};
	bool append{};
	bool hasTarget{};
	unsigned targetUid{};
	float x{};
	float z{};
};

struct UnitOrderRow {
	UnitOrderSaveData data;
	unsigned short orderIndex{};
};

struct AimPathRow {
	unsigned unitUid{};
	bool pending{};
	unsigned short orderIndex{};
	int cell{};
};

struct PlayerLevelSaveData {
	unsigned char player{};
	unsigned char type{};
	unsigned short id{};
	char level{};
};

enum class PendingCommandKind : char {
	CREATION,
	UPGRADE,
	INDIVIDUAL_ORDER,
	GROUP_ORDER,
	FORMATION_ORDER,
	BUILDING_ACTION,
	RESOURCE_ACTION,
	GENERAL_ACTION
};

struct PendingCommandSaveData {
	unsigned short order{};
	PendingCommandKind kind{};
	char action{};
	char actionType{};
	unsigned short id{};
	char player{};
	char level{};
	unsigned number{};
	float x{};
	float z{};
	float hp{};
	unsigned targetUid{};
	short formationId{};
	bool append{};
	std::vector<unsigned> entityUids;
};

struct UnitRuntimeSaveData {
	unsigned uid{};
	unsigned targetUid{};
	unsigned pendingTargetUid{};
	char nextState{};
	bool stateChangePending{};
	unsigned short currentFrameState{};
	short formation{};
	short posInState{};
	float commandPriority{};
	unsigned char commandCenter{};
	float chargeEnergy{};
	AimSaveData aim;
	AimSaveData pendingAim;
	std::vector<UnitOrderSaveData> orders;
};

struct UnitVariableSaveData {
	std::vector<UnitOrderSaveData> orders;
	std::vector<int> aimPath;
	std::vector<int> pendingAimPath;
};

struct ProjectileSaveData {
	unsigned aimUid{};
	float percentToGo{};
	float speed{};
	float attackVal{};
	char player{};
};

struct FormationSaveData {
	short id{};
	char state{};
	char type{};
	float directionX{1.f};
	float directionZ{1.f};
};

struct FrameSaveData {
	unsigned totalTicks{};
};

struct RandSaveData {
	unsigned seed{};
	std::vector<int> floatIndexes;
	std::vector<int> intIndexes;
};

struct ConfigSaveData {
	int precision{};
	int map{};
	int size{};
	unsigned totalTicks{};
	bool randomPresent{};
	RandSaveData random;
};

static constexpr size_t AI_MILITARY_PRESSURE_COUNT = 21;

struct AiSaveData {
	unsigned char player{};
	int prevScore{};
	int prevEnemyScore{};
	unsigned prevUnits{};
	float prevResSum{};
	float prevGatherSum{};
	float foodPriority{};
	float woodPriority{};
	float stonePriority{};
	float goldPriority{};
	std::array<float, AI_MILITARY_PRESSURE_COUNT> militaryPressure{};
	std::array<float, RESOURCES_SIZE> lackingPerResource{};
};

struct ResourcesSaveData {
	unsigned char player{};
	std::array<float, RESOURCES_SIZE> gatherSpeeds1s{};
	std::array<float, RESOURCES_SIZE> sumGatherSpeed{};
	std::array<float, RESOURCES_SIZE> sumValues{};
};

struct AiHistorySaveData {
	unsigned char player{};
	bool action{};
	unsigned tick{};
	char type{};
	char result{};
	unsigned chosenId{};
};

struct AiHistoryRow {
	AiHistorySaveData data;
	unsigned short orderIndex{};
};

struct AiWantSaveData {
	unsigned char player{};
	float priority{};
	float basePriority{};
	unsigned char type{};
	unsigned char count{};
	short specificId{};
	unsigned short age{};
	unsigned char reserveTicks{};
	bool active{};
};

struct AiWantRow {
	AiWantSaveData data;
	unsigned short orderIndex{};
};

struct FormationOrderSaveData {
	short formationId{};
	short action{};
	bool append{};
	bool hasTarget{};
	unsigned targetUid{};
	float x{};
	float z{};
};

struct FormationOrderRow {
	FormationOrderSaveData data;
	unsigned short orderIndex{};
	bool pending{};
};

struct PendingCommandEntityRow {
	unsigned short commandIndex{};
	unsigned short orderIndex{};
	unsigned uid{};
};
