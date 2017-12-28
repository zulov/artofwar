#include "EntityFactory.h"
#include "defines.h"

EntityFactory::EntityFactory() {
}

EntityFactory::~EntityFactory() {
}

float EntityFactory::getSpecSize(SpacingType spacing) {
	switch (spacing) {
	case CONSTANT:
	case RANDOM:
		return INIT_SPACE;
	}
}
