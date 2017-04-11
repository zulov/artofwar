#pragma once
#include <vector>
class Entity;

namespace Urho3D {
	class Node;
}

class AbstractObjectManager
{
public:
	AbstractObjectManager();
	~AbstractObjectManager();

	void AbstractObjectManager::createLink(Urho3D::Node* node, Entity* entity);
	virtual void add(Entity* entity);

	std::vector<Entity*> *entities;
};
