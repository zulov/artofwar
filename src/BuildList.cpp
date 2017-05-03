#include "BuildList.h"
#include "BuildCommand.h"


BuildList::BuildList() {
}

void BuildList::setBuildList(LevelBuilder* _levelBuilder) {
	levelBuilder = _levelBuilder;
}

BuildList::~BuildList() {
}

void BuildList::setParemeters(AbstractCommand* command) {
	BuildCommand *m = static_cast<BuildCommand *>(command);
	m->setLevelBuilder(levelBuilder);
}
