#pragma once
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Button.h>
#include "Game.h"
#include <Urho3D/Graphics/Texture2D.h>

static Urho3D::Button* simpleButton(Urho3D::XMLFile* style, Urho3D::Sprite* sprite, const String& styleName) {
	Urho3D::Button* button = new Urho3D::Button(Game::get()->getContext());
	button->SetStyle(styleName, style);
	
	button->AddChild(sprite);
	return button;
}

static Urho3D::Sprite* createSprite(Texture2D* texture, int sizeX, int sizeY) {
	Urho3D::Sprite* sprite = new Sprite(Game::get()->getContext());

	sprite->SetTexture(texture);
	int textureWidth = texture->GetWidth();
	int textureHeight = texture->GetHeight();
	double scaleX = (sizeX) / (double)textureWidth;
	double scaleY = (sizeY) / (double)textureHeight;
	if (scaleX < scaleY) {
		sprite->SetScale(scaleX);
	} else {
		sprite->SetScale(scaleY);
	}

	sprite->SetSize(textureWidth, textureHeight);
	sprite->SetHotSpot(textureWidth / 2, textureHeight / 2);
	sprite->SetAlignment(HA_CENTER, VA_CENTER);
	sprite->SetOpacity(0.9f);
	return sprite;
}
