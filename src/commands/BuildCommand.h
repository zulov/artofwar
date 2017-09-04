#pragma once
#include "AbstractCommand.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include "BuildingType.h"
#include "SpacingType.h"
#include "Building.h"
#include "SceneObjectManager.h"

namespace Urho3D {
	class Vector3;
}

class BuildCommand:public AbstractCommand
{
public:
	BuildCommand();
	~BuildCommand();
	void execute() override;
	void setSceneObjectManager(SceneObjectManager* _sceneObjectManager);
private:
	SceneObjectManager* sceneObjectManager;

};
