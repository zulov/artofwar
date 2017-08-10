#pragma once
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Button.h>
#include "Game.h"
#include <Urho3D/Graphics/Texture2D.h>
#include "MySprite.h"

static Urho3D::Button* simpleButton(MySprite* sprite, Urho3D::XMLFile* style, const String& styleName) {
	Urho3D::Button* button = new Urho3D::Button(Game::get()->getContext());
	button->SetStyle(styleName, style);

	button->AddChild(sprite);
	return button;
}

static MySprite* createEmptySprite(Urho3D::XMLFile* style, const String& styleName) {
	MySprite* sprite = new MySprite(Game::get()->getContext());

	sprite->SetStyle(styleName, style);
	return sprite;
}

static void setTextureToSprite(MySprite* sprite, Texture2D* texture) {
	sprite->SetTexture(texture);
	int textureWidth = texture->GetWidth();
	int textureHeight = texture->GetHeight();
	//IntVector2 size = sprite->GetSize();
	IntVector2 size = sprite->getMySize();
	double scaleX = (size.x_) / (double)textureWidth;
	double scaleY = (size.y_) / (double)textureHeight;
	sprite->SetScale(1);
	if (scaleX < scaleY) {
		sprite->SetScale(scaleX);
	} else {
		sprite->SetScale(scaleY);
	}

	sprite->SetSize(textureWidth, textureHeight);
	Vector2 perHotSpot = sprite->getPercentHotSpot();
	
	sprite->SetHotSpot(textureWidth * perHotSpot.x_, textureHeight * perHotSpot.y_);
}


static MySprite* createSprite(Texture2D* texture, Urho3D::XMLFile* style, const String& styleName) {
	MySprite* sprite = createEmptySprite(style, styleName);
	setTextureToSprite(sprite, texture);

	return sprite;
}
