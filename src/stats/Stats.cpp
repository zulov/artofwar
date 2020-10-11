#include "Stats.h"

#include <fstream>
#include "AiInputProvider.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/ResourceActionType.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "objects/unit/order/UnitOrder.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "simulation/env/Environment.h"
#include "utils/StringUtils.h"


Stats::Stats() {
	clearCounters();
}

Stats::~Stats() {
	saveAll(1);
}

void Stats::clearCounters() {
	std::fill_n(workersCreatedCount, MAX_PLAYERS, 0);
	std::fill_n(buildingsCreatedCount, MAX_PLAYERS, 0);
	std::fill_n(unitsCreatedCount, MAX_PLAYERS, 0);
}

void Stats::add(GeneralActionCommand* command) {
	// upgrade buildingu
	const auto playerId = command->player;

	const std::string input = join(Game::getAiInputProvider()->getBasicInput(playerId));

	if (command->action == GeneralActionType::BUILDING_LEVEL) {
		// auto opt = Game::getPlayersMan()
		//            ->getPlayer(command->player)->getNextLevelForBuilding(command->id);
		// if (opt.has_value()) {
		// 	auto& createOutput = opt.value()->dbBuildingMetricUp->getParamsNormAsString();
		// 	joinAndPush(buildLevelUpId, player, input, createOutput);
		// }
	}
}

void Stats::add(BuildingActionCommand* command) {
	const std::string input = join(Game::getAiInputProvider()->getBasicInput(command->player));

	auto player = Game::getPlayersMan()->getPlayer(command->player);

	if (command->action == BuildingActionType::UNIT_CREATE) {
		auto level = player->getLevelForUnit(command->id);
		if (level->canCollect) {
			//TODO czy to worker
			workersCreatedCount[command->player] += command->buildings.size();
			for (auto building : command->buildings) {
			auto resInput=	join(Game::getAiInputProvider()->getResourceInput(command->player));
			joinAndPush(whereWorkersCreate, command->player, resInput, getCreateUnitPosOutput(building));
		}
		} else {
			unitsCreatedCount[command->player] += command->buildings.size();
			for (auto building : command->buildings) {
			auto& createOutput = level->dbUnitMetric->getParamsNormAsString();
			joinAndPush(whatBuildingCreate, command->player, input, createOutput);
			const std::string inputWithAiProps = input + ";" + createOutput;
			joinAndPush(whereBuildingCreate, command->player, inputWithAiProps, getCreateUnitPosOutput(building));
		}
		}
		
	}
}

void Stats::add(CreationCommand* command) {
	assert(command->objectType == ObjectType::BUILDING);


	const std::string input = join(Game::getAiInputProvider()->getBuildingsInput(command->player));

	const auto player = Game::getPlayersMan()->getPlayer(command->player);

	auto& createOutput = player->getLevelForBuilding(command->id)
	                           ->dbBuildingMetricPerNation[player->getNation()]->getParamsNormAsString();
	joinAndPush(whatBuildingCreate, command->player, input, createOutput);
	const std::string inputWithAiProps = input + ";" + createOutput;
	joinAndPush(whereBuildingCreate, command->player, inputWithAiProps, getCreateBuildingPosOutput(command));
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

void Stats::saveAll(int size) {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, ifWorkersCreate, "workersCreate", size);
		saveBatch(i, whereWorkersCreate, "whereWorkersCreate", size);

		saveBatch(i, ifBuildingCreate, "workersCreate", size);
		saveBatch(i, whatBuildingCreate, "workersCreate", size);
		saveBatch(i, whereBuildingCreate, "workersCreate", size);

		saveBatch(i, ifUnitCreate, "workersCreate", size);
		saveBatch(i, whatUnitCreate, "workersCreate", size);
		saveBatch(i, whereUnitCreate, "workersCreate", size);
	}
}

void Stats::save(bool accumulate) {
	if (accumulate) {
		for (char i = 0; i < MAX_PLAYERS; ++i) {
			joinAndPush(ifWorkersCreate, i,
			            join(Game::getAiInputProvider()->getResourceInput(i)),
			            std::to_string(workersCreatedCount[i]));
			joinAndPush(ifBuildingCreate, i,
			            join(Game::getAiInputProvider()->getBuildingsInput(i)),
			            std::to_string(buildingsCreatedCount[i]));
			joinAndPush(ifUnitCreate, i,
			            join(Game::getAiInputProvider()->getUnitsInput(i)),
			            std::to_string(unitsCreatedCount[i]));
		}

		clearCounters();
	}
	saveAll(SAVE_BATCH_SIZE);
}

std::string Stats::getCreateBuildingPosOutput(CreationCommand* command) const {
	auto& data = Game::getEnvironment()->getInfluenceDataAt(command->player, command->position);

	return join(data.begin(), data.end());
}

std::string Stats::getCreateUnitPosOutput(Building* building) const {
	const auto buildingPos = building->getPosition();

	auto& data = Game::getEnvironment()->getInfluenceDataAt(building->getPlayer(), {buildingPos.x_, buildingPos.z_});
	return join(data.begin(), data.end());
}

std::string Stats::getResourceIdOutput(UnitActionCommand* command) const {
	float output[4];
	std::fill_n(output, 4, 0.f);
	output[command->order->getToUseId()] = 1;
	return join(output, output + 4);
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
