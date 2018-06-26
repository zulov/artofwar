#pragma once
#include <vector>

class Entity;

class SceneObjectManager
{
public:
	SceneObjectManager();
	~SceneObjectManager();

	void add(Entity* entity);
private:
	std::vector<Entity*> entities;

};
