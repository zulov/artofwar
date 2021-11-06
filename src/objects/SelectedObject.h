#pragma once

namespace Urho3D
{
	struct Billboard;
}

class SelectedObject {
public:
	void disableBillboard(Urho3D::Billboard* billboard);
	void disableBillboards();
	Urho3D::Billboard* getHealthBar() const { return healthBar; }
	void set(Urho3D::Billboard* bar);
private:
	Urho3D::Billboard* healthBar{};
};
