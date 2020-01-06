#pragma once
class AbstractCommand {
public:
	virtual ~AbstractCommand();
	virtual void execute() =0;

protected:
	explicit AbstractCommand(char player): player(player) {
	}

	char player;
};
