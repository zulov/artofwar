#pragma once
#include "commands/PhysicalCommand.h"

enum class GeneralActionType : unsigned char;

class GeneralActionCommand : public PhysicalCommand {
public:
	GeneralActionCommand(short id, GeneralActionType action, char player);

	void execute() override;
private:
	short id;
	GeneralActionType action;
	char player;
};
