#include "BuildList.h"


BuildList::BuildList() {
}

BuildList::~BuildList() {
}

void BuildList::setSceneObjectManager(SceneObjectManager* _sceneObjectManager) {
	sceneObjectManager = _sceneObjectManager;
}

void BuildList::setParemeters(AbstractCommand* command) {
	BuildCommand* m = static_cast<BuildCommand *>(command);
	m->setSceneObjectManager(sceneObjectManager);
}

