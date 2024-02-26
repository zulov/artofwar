#pragma once
#include <vector>

class PhysicalCommand;

class CommandList {
public:
	CommandList() = default;
	virtual ~CommandList();
	void add(PhysicalCommand* command);
	void add(PhysicalCommand* first, PhysicalCommand* second);

	void execute();
private:
	std::vector<PhysicalCommand*> commands;
};
