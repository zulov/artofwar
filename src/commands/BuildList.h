#pragma once
#include "CommandList.h"
#include "scene/SceneObjectManager.h"
#include "BuildCommand.h"


class BuildList:public CommandList
{
public:
	BuildList();
	~BuildList();
	void setSceneObjectManager(SceneObjectManager * _sceneObjectManager);
	virtual void setParemeters(AbstractCommand* command) override;

private:
	SceneObjectManager * sceneObjectManager;
};
