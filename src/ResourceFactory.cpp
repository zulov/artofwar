#include "ResourceFactory.h"
#include "Game.h"
#include "Building.h"


ResourceFactory::ResourceFactory()
{
}


ResourceFactory::~ResourceFactory()
{
}

std::vector<ResourceEntity*>* ResourceFactory::create(unsigned number, ResourceType type, Vector3* center, SpacingType spacing) {
	std::vector<ResourceEntity*>* resources = new std::vector<ResourceEntity *>();
	resources->reserve(number);

	db_resource* dbResource = Game::get()->getDatabaseCache()->getResource(type);
	{
		Node* node = Game::get()->getScene()->CreateChild();

		node->SetPosition((*center));
		StaticModel* planeObject = node->CreateComponent<StaticModel>();
		planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/" + dbResource->model));
		for (int i = 0; i < dbResource->texture.Size(); ++i) {
			planeObject->SetMaterial(i, Game::get()->getCache()->GetResource<Material>("Materials/" + dbResource->texture[i]));
		}

		ResourceEntity* entity = new ResourceEntity(new Vector3(), node);
		entity->populate(dbResource);
		resources->push_back(entity);
	}
	return resources;
}
