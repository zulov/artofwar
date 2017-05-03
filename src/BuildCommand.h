#pragma once
#include "AbstractCommand.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include "Game.h"
#include "LevelBuilder.h"


namespace Urho3D {
	class Vector3;
}

class BuildCommand:public AbstractCommand
{
public:
	BuildCommand(Urho3D::Vector3 _pos);
	~BuildCommand();
	virtual void execute() override;
	void setLevelBuilder(LevelBuilder* _levelBuilder);
private:
	LevelBuilder * levelBuilder;
	Urho3D::Vector3 * position;
};
