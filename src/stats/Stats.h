#pragma once
#include <vector>
#include <string>
#include "defines.h"

#define INPUT_STATS_SIZE 24
#define STATS_PER_PLAYER_SIZE 12
#define SAVE_BATCH_SIZE 30

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
private:
	void update(short id);
	void clear();
	std::string getOutput(UpgradeCommand* command);
	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[STATS_PER_PLAYER_SIZE];
	std::vector<std::string> dataToSavePerPlayer[MAX_PLAYERS];
};