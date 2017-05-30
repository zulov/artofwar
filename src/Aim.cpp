#include "Aim.h"
#include "Game.h"
#include <Urho3D/Graphics/StaticModel.h>


Aim::Aim(Urho3D::Vector3* _position) {
	position = new Urho3D::Vector3(*_position);
	position->y_ = 2;
	Game* game = Game::get();
	node = game->getScene()->CreateChild("Box");
	node->SetPosition(*position);
	node->SetRotation(Urho3D::Quaternion(0, 0, 180));

	Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
	model->SetModel(game->getCache()->GetResource<Urho3D::Model>("Models/aim.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/red.xml"));
	position->y_ = 0;
	radius = 3;
}

Aim::~Aim() {
	node->RemoveAllComponents();
	node->Remove();

	delete position;
}

Urho3D::Vector3* Aim::getPosition() {
	return position;
}

double Aim::getRadius() {
	return radius;
}
