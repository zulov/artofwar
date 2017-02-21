#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Profiler.h>
#include <Urho3D/Engine/Engine.h>

#include <Urho3D/Graphics/Renderer.h>

#include <Urho3D/Input/Input.h>

#include <Urho3D/DebugNew.h>

#include "Simulation.h"


Simulation::Simulation(Context* context) {

	envStrategy = new EnviromentStrategy();
	forceStrategy = new ForceStrategy();
	srand(time(NULL));
}

void Simulation::setUnits(std::vector<Unit*>* units) {
	this->units = units;
	envStrategy->populate(units);
}

void Simulation::animateObjects(float timeStep) {
	//URHO3D_PROFILE(animateObjects);
	calculateForces();
	moveUnits(timeStep);
}

void Simulation::update(Input* input, float timeStep) {
	using namespace Update;

	//float timeStep = eventData[P_TIMESTEP].GetFloat();

	//Input* input = GetSubsystem<Input>();
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	reset();

	if (animate) {
		animateObjects(timeStep);
	}

}

void Simulation::moveUnits(float timeStep) {
	for (unsigned i = 0; i < units->size(); ++i) {
		units->at(i)->applyForce(timeStep);
		units->at(i)->move(timeStep);
		envStrategy->update(units->at(i));
	}
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		std::vector<Unit*>* neighbours = envStrategy->getNeighbours((*units)[i], units);

		Vector3 sepPedestrian = forceStrategy->separationUnits((*units)[i], neighbours);
		Vector3 sepObstacle = forceStrategy->separationObstacle((*units)[i], 0);

		Vector3 destForce = Vector3();// forceStrategy->destinationForce((*units)[i]);
		Vector3 rand = forceStrategy->randomForce();

		Vector3 forces = sepPedestrian += sepObstacle += destForce += rand;

		(*units)[i]->setAcceleration(forces);

		delete neighbours;
	}
}

void Simulation::reset() {
//	Input* input = GetSubsystem<Input>();
//
//	if (input->GetKeyDown(KEY_P)) {
//		resetUnits();
//	}
}

void Simulation::resetUnits() {
//	for (int i = 0; i < units->size(); i++) {
//		scene->RemoveChild((*units)[i]->getNode());
//		delete (*units)[i];
//	}
//	units->clear();
//	createUnits(edgeSize, spaceSize);
}





