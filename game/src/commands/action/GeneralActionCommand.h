#pragma once
#include "commands/PhysicalCommand.h"

enum class GeneralActionType : unsigned char;
struct PendingCommandSaveData;

class GeneralActionCommand : public PhysicalCommand {
public:
	GeneralActionCommand(short id, GeneralActionType action, unsigned char playerId);

	void execute() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

private:
	short id;
	GeneralActionType action;
	unsigned char playerId;
};
