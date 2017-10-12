#include "Static.h"
#include <Urho3D/Graphics/StaticModel.h>
#include "Game.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>

Static::Static(Vector3* _position, ObjectType _type) : Physical(_position, _type) {
	plane = node->CreateChild();
	plane->Translate(Vector3::UP * 3, TS_PARENT);
	StaticModel* planeObject = plane->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/Box.mdl"));
	planeObject->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue.xml"));
}

Static::~Static() {
	plane->Remove();
}

IntVector2& Static::getGridSize() {
	return gridSize;
}
