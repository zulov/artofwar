#include "Static.h"
#include "Game.h"


Static::Static(Vector3* _position, ObjectType _type) : Physical(_position, _type) {
	
}

Static::~Static() {
}

IntVector2& Static::getGridSize() {
	return gridSize;
}
