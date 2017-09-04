#pragma once
#include <vector>
#include "objects/Entity.h"

class SceneObjectManager
{
public:
	SceneObjectManager();
	~SceneObjectManager();

	void add(Entity* entity);
private:
	std::vector<Entity*> *entities;

};
