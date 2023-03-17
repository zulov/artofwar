#pragma once
#include "commands/AbstractCommand.h"

enum class GeneralActionType : char;

class GeneralActionCommand : public AbstractCommand {
	friend class Stats;
public:
	GeneralActionCommand(short id, GeneralActionType action, char player);

	void clean() override;
	bool expired() override;
	void execute() override;
private:
	short id;
	GeneralActionType action;
};
