#pragma once
#include "objects/Entity.h"
#include <vector>


class SceneObjectManager
{
public:
	SceneObjectManager();
	~SceneObjectManager();

	void add(Entity* entity);
private:
	std::vector<Entity*> entities;

};
