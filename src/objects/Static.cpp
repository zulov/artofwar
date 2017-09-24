#include "Static.h"


Static::Static(Vector3* _position, Urho3D::Node* _node, ObjectType _type) : Physical(_position, _node, _type) {

}


Static::~Static() {
}

IntVector2& Static::getGridSize() {
	return gridSize;
}
