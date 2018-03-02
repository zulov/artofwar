#pragma once
#include "d:\Programowanie\vs2015\art_of_war\src\commands\AbstractCommand.h"
#include "objects/queue/QueueType.h"

class UpgradeCommand :
	public AbstractCommand
{
public:
	UpgradeCommand(int _player, int _id, QueueType _type);
	~UpgradeCommand() override;
	void execute() override;
private:
	int player;
	int id;
	QueueType type;
};
