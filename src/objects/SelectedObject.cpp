#include "SelectedObject.h"

#include <Urho3D/Graphics/BillboardSet.h>

void SelectedObject::disableBillboards() {
	if (healthBar) {
		healthBar->enabled_ = false;
		healthBar = nullptr;
	}
}

void SelectedObject::set(Urho3D::Billboard* bar) {
	this->healthBar = bar;
}
