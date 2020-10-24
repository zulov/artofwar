#pragma once
class AbstractCommand {
public:
	virtual ~AbstractCommand() = default;
	virtual void execute() =0;
	virtual bool expired() { return false; }
	virtual void clean() {}

protected:
	explicit AbstractCommand(char player): player(player) { }
	char player;
};
