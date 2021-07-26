#include "Stats.h"

#include <fstream>
#include "AiInputProvider.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "objects/building/Building.h"
#include "objects/unit/order/UnitOrder.h"
#include "objects/unit/order/enums/UnitAction.h"
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

void Stats::init() {
	setInputs();
}

void Stats::clearCounters() {
	std::fill_n(workersCreatedCount, MAX_PLAYERS, 0);
	std::fill_n(buildingsCreatedCount, MAX_PLAYERS, 0);
	std::fill_n(unitsCreatedCount, MAX_PLAYERS, 0);
}

void Stats::setInputs() {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		ifWorkersCreateInput[i] = join(Game::getAiInputProvider()->getResourceInput(i));
		ifBuildingsCreatedInput[i] = join(Game::getAiInputProvider()->getBuildingsInput(i));
		ifUnitsCreatedInput[i] = join(Game::getAiInputProvider()->getUnitsInput(i));
	}
}

void Stats::add(GeneralActionCommand* command) {
	if (!SIM_GLOBALS.AI_OUTPUT) { return; }
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
	if (!SIM_GLOBALS.AI_OUTPUT) { return; }
	auto player = Game::getPlayersMan()->getPlayer(command->player);

	if (command->action == BuildingActionType::UNIT_CREATE) {
		auto level = player->getLevelForUnit(command->id);
		const std::string resInput = join(Game::getAiInputProvider()->getResourceInput(command->player));
		if (level->canCollect) {
			//TODO czy to worker
			workersCreatedCount[command->player] += command->buildings.size();
			for (auto building : command->buildings) {
				joinAndPush(whereWorkersCreate, command->player, resInput, getCreateUnitPosOutput(building));
			}
		} else {
			unitsCreatedCount[command->player] += command->buildings.size();
			const auto metric = level->dbUnitMetric;
			const std::string unitsInput = join(Game::getAiInputProvider()->getUnitsInput(command->player));
			joinAndPush(whichUnitCreate, command->player, unitsInput, metric->getParamsNormAsString(),
			            command->buildings.size());

			const std::string inputWithMetric = join(
				Game::getAiInputProvider()->getUnitsInputWithMetric(command->player, metric));
			for (auto building : command->buildings) {
				joinAndPush(whereUnitCreate, command->player, inputWithMetric, getCreateUnitPosOutput(building));
			}
		}

	}
}

void Stats::add(CreationCommand* command) {
	assert(command->objectType == ObjectType::BUILDING);
	if (!SIM_GLOBALS.AI_OUTPUT) { return; }
	const auto player = Game::getPlayersMan()->getPlayer(command->player);

	const std::string input = join(Game::getAiInputProvider()->getBuildingsInput(command->player));

	auto metric = player->getLevelForBuilding(command->id)->dbBuildingMetricPerNation[player->getNation()];
	++buildingsCreatedCount[command->player];

	joinAndPush(whichBuildingCreate, command->player, input, metric->getParamsNormAsString());

	const std::string inputWithMetric = join(
		Game::getAiInputProvider()->getBuildingsInputWithMetric(command->player, metric));
	joinAndPush(whereBuildingCreate, command->player, inputWithMetric, getCreateBuildingPosOutput(command));
}

void Stats::joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output,
                        int number) {
	input.append(";;").append(output);
	for (int i = 0; i < number; ++i) {
		array[player].push_back(input);
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
	if (!SIM_GLOBALS.AI_OUTPUT) { return; }
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, ifWorkersCreate, "worker/ifWorkerCreate", size);
		saveBatch(i, whereWorkersCreate, "worker/whereWorkerCreate", size);

		saveBatch(i, ifBuildingCreate, "building/ifBuildingCreate", size);
		saveBatch(i, whichBuildingCreate, "building/whichBuildingCreate", size);
		saveBatch(i, whereBuildingCreate, "building/whereBuildingCreate", size);

		saveBatch(i, ifUnitCreate, "unit/ifUnitCreate", size);
		saveBatch(i, whichUnitCreate, "unit/whichUnitCreate", size);
		saveBatch(i, whereUnitCreate, "unit/whereUnitCreate", size);

		saveBatch(i, whichResource, "resource/whichResource", size);
	}
}

void Stats::save(bool accumulate) {
	if (!SIM_GLOBALS.AI_OUTPUT) { return; }
	if (accumulate) {
		for (char i = 0; i < MAX_PLAYERS; ++i) {
			//TODO chyba ze jednak srednia a nie poczatek
			joinAndPush(ifWorkersCreate, i, ifWorkersCreateInput[i], std::to_string(workersCreatedCount[i]));
			joinAndPush(ifBuildingCreate, i, ifBuildingsCreatedInput[i], std::to_string(buildingsCreatedCount[i]));
			joinAndPush(ifUnitCreate, i, ifUnitsCreatedInput[i], std::to_string(unitsCreatedCount[i]));
		}

		clearCounters();
		setInputs();
	}
	saveAll(SAVE_BATCH_SIZE);
}

std::string Stats::getCreateBuildingPosOutput(CreationCommand* command) const {
	return getPosOutput(command->player, command->position);
}

std::string Stats::getCreateUnitPosOutput(Building* building) const {
	const auto buildingPos = building->getPosition();
	auto pos = Urho3D::Vector2(buildingPos.x_, buildingPos.z_);
	return getPosOutput(building->getPlayer(), pos);
}

std::string Stats::getPosOutput(char player, Urho3D::Vector2& pos) const {
	auto& data = Game::getEnvironment()->getInfluenceDataAt(player, pos);
	return join(data.begin(), data.end());
}

void Stats::add(UnitActionCommand* command) {
	const auto playerId = command->player;
	//TODO czy tu wszystkie rozkazy czy tylko te wydane bez posrednio a nie same
	if (command->order->getId() == cast(UnitAction::COLLECT)) {
		auto input = join(Game::getAiInputProvider()->getResourceInput(playerId));
		joinAndPush(whichResource, playerId, input, getResourceIdOutput(command), command->order->getSize());
	}
}

std::string Stats::getResourceIdOutput(UnitActionCommand* command) const {
	float output[4];
	std::fill_n(output, 4, 0.f);
	output[command->order->getToUseId()] = 1.f;
	return join(output, output + 4);
}
