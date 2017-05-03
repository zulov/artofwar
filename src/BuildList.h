#pragma once
#include "CommandList.h"
#include "LevelBuilder.h"
class LevelBuilder;
class BuildList:public CommandList
{
public:
	BuildList();
	~BuildList();
	void setBuildList(LevelBuilder* _levelBuilder);
	virtual void setParemeters(AbstractCommand* command) override;

private:
	LevelBuilder * levelBuilder;
};
