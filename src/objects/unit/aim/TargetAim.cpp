#include "TargetAim.h"
#include "Game.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include "../Unit.h"


TargetAim::TargetAim(Urho3D::Vector3* _position) {
	position = new Urho3D::Vector3(*_position);
	position->y_ = 2;
	Game* game = Game::get();
	node = game->getScene()->CreateChild();
	node->SetPosition(*position);
	node->SetRotation(Urho3D::Quaternion(0, 0, 180));

	Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
	model->SetModel(game->getCache()->GetResource<Urho3D::Model>("Models/aim.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/red.xml"));
	position->y_ = 0;
	radius = 3;
}


TargetAim::~TargetAim() {
	node->RemoveAllComponents();
	node->Remove();

	delete position;
}

Urho3D::Vector3* TargetAim::getDirection(Unit* unit) {
	return new Vector3((*(position)) - (*unit->getPosition()));
}

bool TargetAim::ifReach(Unit* unit) {
	float dist = ((*unit->getPosition()) - (*position)).LengthSquared();
	if (dist < radius * radius) {
		return true;
	}
	return false;
}
