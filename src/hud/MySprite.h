#pragma once

#include <Urho3D/UI/Sprite.h>

using namespace Urho3D;

class MySprite : public Sprite
{
	URHO3D_OBJECT(MySprite, Sprite);
public:
	MySprite(Urho3D::Context* context);
	~MySprite();
	static void RegisterObject(Urho3D::Context* context);
	const Vector2& getPercentHotSpot() const;
	const IntVector2& getMySize() const;
private:
	Urho3D::Vector2 percentHotspot;
	Urho3D::IntVector2 mySize;
};
