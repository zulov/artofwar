#pragma once

#include <Urho3D/UI/Sprite.h>

class MySprite : public Urho3D::Sprite {
URHO3D_OBJECT(MySprite, Sprite)

	explicit MySprite(Urho3D::Context* context);
	~MySprite() override;
	static void RegisterObject(Urho3D::Context* context);
	const Urho3D::Vector2& getPercentHotSpot() const;
	const Urho3D::IntVector2& getMySize() const;
private:
	Urho3D::Vector2 percentHotspot;
	Urho3D::IntVector2 mySize;
};
