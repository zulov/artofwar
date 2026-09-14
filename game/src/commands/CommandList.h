#pragma once
#include <vector>

#include "scene/load/RuntimeSaveData.h"

class PhysicalCommand;

class CommandList {
public:
	CommandList() = default;
	~CommandList();
	void add(PhysicalCommand* command);
	void add(PhysicalCommand* first, PhysicalCommand* second);

	void execute();
	std::vector<PendingCommandSaveData> saveState(unsigned short& nextOrder) const;

private:
	std::vector<PhysicalCommand*> commands;
};
