#include "ResourceFactory.h"
#include "Game.h"


ResourceFactory::ResourceFactory()
{
}


ResourceFactory::~ResourceFactory()
{
}

ResourceEntity* ResourceFactory::createResource(ResourceType type, Vector3* pos) {
	db_resource* dbResource = Game::get()->getDatabaseCache()->getResource(type);

	Node* node = Game::get()->getScene()->CreateChild();

	node->SetPosition((*pos));
	StaticModel* planeObject = node->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/" + dbResource->model));
	for (int i = 0; i < dbResource->texture.Size(); ++i) {
		planeObject->SetMaterial(i, Game::get()->getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
	}

	ResourceEntity* entity = new ResourceEntity(new Vector3(), node);
	entity->populate(dbResource);
	return entity;
}