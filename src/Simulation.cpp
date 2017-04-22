#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "Simulation.h"


Simulation::Simulation(EnviromentStrategy *_enviromentStrategy) {
	envStrategy = _enviromentStrategy;
	forceStrategy = new ForceStrategy();
	objectManager = new SimulationObjectManager();
	srand(time(NULL));
	animate = true;
}


void Simulation::createUnits() {
	units = objectManager->createUnits();
	envStrategy->populate(units);
}

void Simulation::animateObjects(float timeStep) {
	calculateForces();
	moveUnits(timeStep);
}

void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		animateObjects(timeStep);
	}
}

void Simulation::moveUnits(float timeStep) {
	timeStep = 0.05;
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit * unit = (*units)[i];
		unit->applyForce(timeStep);
		unit->move(timeStep);
		envStrategy->update(unit);
	}
	envStrategy->clear();
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit * unit = (*units)[i];
		std::vector<Entity*>* neighbours = envStrategy->getNeighbours(unit, units);

		Vector3* sepPedestrian = forceStrategy->separationUnits(unit, neighbours);
		Vector3* sepObstacle = forceStrategy->separationObstacle(unit, 0);

		Vector3* destForce = forceStrategy->destination(unit);
		Vector3* rand = forceStrategy->randomForce();

		Vector3* forces = new Vector3((*sepPedestrian) += (*sepObstacle) += (*destForce) += (*rand));
		delete sepPedestrian;
		delete sepObstacle;
		delete destForce;
		delete rand;

		unit->setAcceleration(forces);

		delete neighbours;
	}
}
