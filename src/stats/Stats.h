#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "commands/action/ActionCommand.h"

#define INPUT_STATS_SIZE 24
#define STATS_PER_PLAYER_SIZE 12
#define SAVE_BATCH_SIZE 10

class AbstractCommand;
class ActionCommand;
class UpgradeCommand;
class CreationCommand;

class Stats {
public:
	Stats();
	~Stats();
	int getScoreFor(short id) const;
	float* getInputFor(short id);
	void init();
	void add(UpgradeCommand* command);
	void add(ActionCommand* command);
	void add(CreationCommand* command);
	void save();
	void addBuildLevel(short id, const ActionParameter& parameter, char playerId);
private:
	void update(short id);
	void clear();
	std::string getOutput(CreationCommand* command) const;
	std::string getOutput(UpgradeCommand* command) const;
	std::string getOutput(ActionCommand* command) const;
	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[STATS_PER_PLAYER_SIZE];
	std::vector<std::string> dataToSavePerPlayer[MAX_PLAYERS];
};
