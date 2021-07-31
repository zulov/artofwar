#include "SelectedObject.h"

#include <Urho3D/Graphics/BillboardSet.h>

void SelectedObject::disableBillboard(Urho3D::Billboard* billboard) {
	if (billboard) {
		billboard->enabled_ = false;
		billboard = nullptr;
	}
}

void SelectedObject::disableBillboards() {
	disableBillboard(healthBar);
	disableBillboard(aura);
}

void SelectedObject::set(Urho3D::Billboard* aura, Urho3D::Billboard* bar) {
	this->aura = aura;
	this->healthBar = bar;
}
