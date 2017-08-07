#pragma once
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Button.h>
#include "Game.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

static Urho3D::Button* simpleButton(Urho3D::Sprite* sprite, Urho3D::XMLFile* style, const String& styleName) {
	Urho3D::Button* button = new Urho3D::Button(Game::get()->getContext());
	button->SetStyle(styleName, style);

	button->AddChild(sprite);
	return button;
}

static Urho3D::Sprite* createEmptySprite(Urho3D::XMLFile* style, const String& styleName) {
	Urho3D::Sprite* sprite = new Sprite(Game::get()->getContext());

	sprite->SetStyle(styleName, style);
	return sprite;
}

static void setTextureToSprite(Sprite* sprite, Texture2D* texture) {
	sprite->SetTexture(texture);
	int textureWidth = texture->GetWidth();
	int textureHeight = texture->GetHeight();
	IntVector2 size = sprite->GetSize();
	double scaleX = (size.x_) / (double)textureWidth;
	double scaleY = (size.y_) / (double)textureHeight;
	if (scaleX < scaleY) {
		sprite->SetScale(scaleX);
	}
	else {
		sprite->SetScale(scaleY);
	}

	sprite->SetSize(textureWidth, textureHeight);
	IntVector2 hotSpot = sprite->GetHotSpot();
	sprite->SetHotSpot(textureWidth * hotSpot.x_ / 100, textureHeight * hotSpot.y_ / 100);
}


static Urho3D::Sprite* createSprite(Texture2D* texture, Urho3D::XMLFile* style, const String& styleName) {
	Urho3D::Sprite* sprite = createEmptySprite(style, styleName);
	setTextureToSprite(sprite, texture);

	return sprite;
}
