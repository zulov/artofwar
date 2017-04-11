#include "AbstractObjectManager.h"
#include "LinkComponent.h"


AbstractObjectManager::AbstractObjectManager()
{
}


AbstractObjectManager::~AbstractObjectManager()
{
}


void AbstractObjectManager::createLink(Node* node, Entity* entity) {
	LinkComponent* lc = node->CreateComponent<LinkComponent>();

	lc->bound(node, entity);
}

void AbstractObjectManager::add(Entity* entity) {
	entities->push_back(entity);
	createLink(entity->getNode(), entity);
}