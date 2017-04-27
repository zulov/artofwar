#include "BuildCommand.h"


BuildCommand::BuildCommand(Urho3D::Vector3 _pos) {
	position = new Urho3D::Vector3(_pos);
	position->y_ = 0;
}


BuildCommand::~BuildCommand() {
	delete position;
}

void BuildCommand::execute() {
	Game* game = Game::get();
	Node* node = game->getScene()->CreateChild("Box");//TODO tego noda trzeba gdzies zwrocic
	node->SetPosition(*position);

	Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
	model->SetModel(game->getCache()->GetResource<Urho3D::Model>("Models/Tower.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/tower.xml"));
}
