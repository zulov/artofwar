#pragma once
#include "commands/PhysicalCommand.h"

enum class GeneralActionType : char;

class GeneralActionCommand : public PhysicalCommand {
	friend class Stats;
public:
	GeneralActionCommand(short id, GeneralActionType action, char player);

	void execute() override;
private:
	short id;
	GeneralActionType action;
	char player;
};
