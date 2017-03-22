#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "Simulation.h"


Simulation::Simulation(Context* context, ResourceCache* _cache, SharedPtr<Scene> _scene) {
	envStrategy = new EnviromentStrategy();
	forceStrategy = new ForceStrategy();
	objectManager = new ObjectManager(_cache, _scene);
	srand(time(NULL));
	animate = false;
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
	using namespace Update;

	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	//reset();

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
	envStrategy->clear();
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		std::vector<Unit*>* neighbours = envStrategy->getNeighbours((*units)[i], units);

		Vector3 *sepPedestrian = forceStrategy->separationUnits((*units)[i], neighbours);
		Vector3 *sepObstacle = forceStrategy->separationObstacle((*units)[i], 0);

		Vector3 *destForce =  forceStrategy->destination((*units)[i]);
		Vector3 *rand = forceStrategy->randomForce();

		Vector3 forces = (*sepPedestrian) += (*sepObstacle) += (*destForce) += (*rand);
		delete sepPedestrian;
		delete sepObstacle;
		delete destForce;
		delete rand;

		(*units)[i]->setAcceleration(forces);

		delete neighbours;
	}
}
