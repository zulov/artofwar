#pragma once

struct PendingCommandSaveData;

class PhysicalCommand {
public:
	virtual ~PhysicalCommand() = default;
	virtual void execute() = 0;
	virtual PendingCommandSaveData saveState(unsigned short order) const = 0;
};
