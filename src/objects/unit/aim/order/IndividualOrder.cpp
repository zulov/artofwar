#include "IndividualOrder.h"


IndividualOrder::IndividualOrder(Unit* unit, const Urho3D::Vector2& vector,
                                 const Physical* physical, UnitOrder action): FutureOrder(vector, physical, action),
	unit(unit) {
}


IndividualOrder::~IndividualOrder() {
}
