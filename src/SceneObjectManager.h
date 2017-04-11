#pragma once
#include "Entity.h"
#include <vector>
#include "AbstractObjectManager.h"

class SceneObjectManager :public AbstractObjectManager
{
public:
	SceneObjectManager();
	~SceneObjectManager();

	void add(Entity* entity) override;
};

