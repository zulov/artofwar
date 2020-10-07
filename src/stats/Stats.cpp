#include "Stats.h"
#include <fstream>
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/ResourceActionType.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "objects/ObjectEnums.h"
#include "objects/unit/order/UnitOrder.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "simulation/env/Environment.h"
#include "utils/StringUtils.h"

constexpr float DEFAULT_NORMALIZE_VALUE = 10.f;
constexpr char INFLUENCE_DATA_SIZE = 9; //TODO hard code

Stats::Stats() {
	std::fill_n(workersCreatedNum, MAX_PLAYERS, 0);
}

Stats::~Stats() {
	saveAll(1, 1);
}

void Stats::init() {


}

std::string Stats::getInputData(char player) {
	return join(getBasicInput(player));
}

void Stats::add(GeneralActionCommand* command) {
	const auto player = command->player;
	//
	// const std::string input = getInputData(player);
	//
	// joinAndPush(mainOrder, player, input, getOutput(command));
	//
	// auto opt = Game::getPlayersMan()
	//            ->getPlayer(command->player)->getNextLevelForBuilding(command->id);
	// if (opt.has_value()) {
	// 	auto& createOutput = opt.value()->dbBuildingMetricUp->getParamsNormAsString();
	// 	joinAndPush(buildLevelUpId, player, input, createOutput);
	// }
}

void Stats::add(ResourceActionCommand* command) {
	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(mainOrder, player, input, getOutput(command), command->resources.size());
}

void Stats::add(BuildingActionCommand* command) {
	const std::string input = getInputData(command->player);
	const std::string basicOutput = getOutput(command);
	auto player = Game::getPlayersMan()->getPlayer(command->player);

	if (command->action == BuildingActionType::UNIT_CREATE) {
		auto level = Game::getPlayersMan()->getPlayer(command->player)->getLevelForUnit(command->id);
		if (level->canCollect) {
			//TODO czy to worker
			workersCreatedNum[command->player] += command->buildings.size();
		}
	}

	// joinAndPush(mainOrder, command->player, input, basicOutput, command->buildings.size());
	// for (auto building : command->buildings) {
	// 	if (command->action == BuildingActionType::UNIT_CREATE) {
	// 		auto& createOutput = player->getLevelForUnit(command->id)->dbUnitMetric->getParamsNormAsString();
	// 		joinAndPush(unitCreateId, command->player, input, createOutput);
	//
	// 		const std::string inputWithAiProps = input + ";" + createOutput;
	// 		joinAndPush(unitCreatePos, command->player, inputWithAiProps, getCreateUnitPosOutput(building));
	// 	} else if (command->action == BuildingActionType::UNIT_LEVEL) {
	// 		auto opt = player->getNextLevelForUnit(command->id);
	// 		if (opt.has_value()) {
	// 			auto& createOutput = opt.value()->dbUnitMetricUp->getParamsNormAsString();
	// 			joinAndPush(unitUpgradeId, command->player, input, createOutput);
	//
	// 			const std::string inputWithAiProps = input + ";" + createOutput;
	// 			joinAndPush(unitLevelUpPos, command->player, inputWithAiProps, getLevelUpUnitPosOutput(building));
	// 		}
	// 	}
	// }
}

void Stats::add(CreationCommand* command) {
	if (command->objectType != ObjectType::BUILDING) {
		Game::getLog()->Write(0, "ERROR - WRONG command");
	}

	const auto playerId = command->player;

	const std::string input = getInputData(playerId);

	joinAndPush(mainOrder, playerId, input, getOutput(command));
	auto player = Game::getPlayersMan()->getPlayer(command->player);

	auto& createOutput = Game::getPlayersMan()->getPlayer(command->player)
	                                          ->getLevelForBuilding(command->id)
	                                          ->dbBuildingMetricPerNation[player->getNation()]->getParamsNormAsString();
	joinAndPush(buildingCreateId, playerId, input, createOutput);
	const std::string inputWithAiProps = input + ";" + createOutput;
	joinAndPush(buildingCreatePos, playerId, inputWithAiProps, getCreateBuildingPosOutput(command));
}

void Stats::joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output,
                        int number) {
	if (!output.empty()) {
		input.append(";;").append(output);
		for (int i = 0; i < number; ++i) {
			array[player].push_back(input);
		}
	}
}

void Stats::save(int i, std::vector<std::string>* array, std::string fileName) const {
	std::ofstream outFile;

	outFile.open(fileName, std::ios_base::app);
	for (const auto& e : array[i]) {
		outFile << e << "\n";
	}
	outFile.close();
}

void Stats::saveBatch(int i, std::vector<std::string>* array, std::string name, int size) const {
	if (array[i].size() >= size) {
		save(i, array, "result/ai/" + name + "_" + std::to_string(i) + ".csv");
		save(i, array, "result/ai/" + name + "_combine.csv");

		array[i].clear();
	}
}

void Stats::saveAll(int big, int small) {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, mainOrder, "main", big);

		saveBatch(i, buildingCreateId, "buildId", small);
		saveBatch(i, buildingCreatePos, "buildPos", small);

		saveBatch(i, unitCreateId, "unitId", small);
		saveBatch(i, unitCreatePos, "unitPos", small);

		saveBatch(i, unitOrderId, "unitOrderId", small);

		saveBatch(i, buildLevelUpId, "buildLevelUpId", small);

		saveBatch(i, unitUpgradeId, "unitUpgradeId", small);
		saveBatch(i, unitLevelUpPos, "unitLevelUpPos", small);

		saveBatch(i, resourceId, "resourceId", small);
		saveBatch(i, workersCreate, "workersCreate", small);
	}
}

void Stats::save(bool accumulate) {
	if (accumulate) {
		for (char i = 0; i < MAX_PLAYERS; ++i) {
			joinAndPush(workersCreate, i, getResourceInputAsString(i), std::to_string(workersCreatedNum[i]));
		}

		std::fill_n(workersCreatedNum, MAX_PLAYERS, 0);
	}
	saveAll(SAVE_BATCH_SIZE, SAVE_BATCH_SIZE_MINI);
}

int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

std::string Stats::getCreateBuildingPosOutput(CreationCommand* command) const {
	auto& data = Game::getEnvironment()->getInfluenceDataAt(command->player, command->position);

	return join(data.begin(), data.end());
}

std::string Stats::getCreateUnitPosOutput(Building* building) const {
	float output[INFLUENCE_DATA_SIZE + 1];

	std::fill_n(output, INFLUENCE_DATA_SIZE + 1, 0.f);
	auto buildingPos = building->getPosition();

	auto& data = Game::getEnvironment()->getInfluenceDataAt(building->getPlayer(), {buildingPos.x_, buildingPos.z_});
	std::copy(data.begin(), data.end(), output);

	output[INFLUENCE_DATA_SIZE] = building->getQueue()->getSize() / DEFAULT_NORMALIZE_VALUE;
	return join(output, output + INFLUENCE_DATA_SIZE + 1);
}

std::string Stats::getLevelUpUnitPosOutput(Building* building) const {
	auto queueSize = building->getQueue()->getSize() / DEFAULT_NORMALIZE_VALUE;
	std::ostringstream ss;
	ss << queueSize;
	return std::string(ss.str());
}

std::string Stats::getOutput(CreationCommand* command) const {
	return getOutput(StatsOutputType::CREATE_BUILDING);
}

std::string Stats::getOutput(UpgradeCommand* command) const {
	switch (command->type) {
	case QueueActionType::UNIT_LEVEL:
		return getOutput(StatsOutputType::LEVEL_UP_UNIT);
	case QueueActionType::BUILDING_LEVEL:
		return getOutput(StatsOutputType::LEVEL_UP_BUILDING);
	default: ;
	}
}

std::string Stats::getOutput(ResourceActionCommand* command) const {
	//TODO
	switch (command->action) {
	case ResourceActionType::COLLECT:
		return getOutput(StatsOutputType::IDLE);
	case ResourceActionType::CANCEL:
		return getOutput(StatsOutputType::IDLE);
	default: ;
	}
}

std::string Stats::getOutput(BuildingActionCommand* command) const {
	switch (command->action) {
	case BuildingActionType::UNIT_CREATE:
		return getOutput(StatsOutputType::CREATE_UNIT);
	case BuildingActionType::UNIT_LEVEL:
		return getOutput(StatsOutputType::LEVEL_UP_UNIT);
	case BuildingActionType::UNIT_UPGRADE:
		//TODO dodac kiedyś
		break;
	default: ;
	}
}

std::string Stats::getOutput(GeneralActionCommand* command) const {
	return getOutput(StatsOutputType::LEVEL_UP_BUILDING);
}

std::string Stats::getOutput(StatsOutputType stat) const {
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);

	output[cast(stat)] = 1;

	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}

std::string Stats::getResourceIdOutput(UnitActionCommand* command) const {
	float output[4];
	std::fill_n(output, 4, 0.f);
	output[command->order->getToUseId()] = 1;
	return join(output, output + 4);
}

std::string Stats::getResourceInputAsString(char playerId) {
	return join(getResourceInput(playerId));
}

void Stats::add(UnitActionCommand* command) {
	const auto playerId = command->player;
	//
	// const std::string input = getInputData(playerId);
	//
	// joinAndPush(unitOrderId, playerId, input, getOutput(command));
	// if (command->order->getId() == static_cast<char>(UnitAction::COLLECT)) {
	// 	joinAndPush(resourceId, playerId, getResourceIdInputAsString(playerId), getResourceIdOutput(command),
	// 	            command->order->getSize());
	// }
}
