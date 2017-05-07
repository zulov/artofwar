#pragma once


#include "Unit.h"
#include <vector>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
#include "SimulationObjectManager.h"
#include "SimulationCommandList.h"
#include "CommandList.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>


class SimulationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager, CommandList* commandList);
	void update(Input* input, float timeStep);
	void createUnits();

private:
	void animateObjects(float timeStep);
	void moveUnits(float timeStep);
	void calculateForces();

	bool animate;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	EnviromentStrategy* envStrategy;
	ForceStrategy* forceStrategy;
	SimulationObjectManager* simObjectManager;
	SimulationCommandList* simCommandList;
	CommandList* commandList;
};
