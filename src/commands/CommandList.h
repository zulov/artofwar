#pragma once
#include <vector>

class AbstractCommand;

class CommandList {
public:
	CommandList() = default;
	virtual ~CommandList();
	void add(AbstractCommand* command);
	void add(AbstractCommand* first, AbstractCommand* second);

	void execute();
protected:
	virtual void setParameters(AbstractCommand* command);
private:
	std::vector<AbstractCommand*> commands;
};
