#pragma once
class AbstractCommand {
public:
	virtual ~AbstractCommand();
	virtual void execute() =0;

	virtual bool expired() =0;
	virtual void clean() =0;

protected:
	explicit AbstractCommand(char player): player(player) {
	}

	char player;
};
