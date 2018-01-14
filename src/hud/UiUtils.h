#pragma once
#include "Game.h"
#include "MySprite.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>

static Urho3D::Button* simpleButton(MySprite* sprite, Urho3D::XMLFile* style, const String& styleName) {
	Urho3D::Button* button = new Urho3D::Button(Game::get()->getContext());
	button->SetStyle(styleName, style);
	if (sprite) {
		button->AddChild(sprite);
	}
	return button;
}

static MySprite* createEmptySprite(Urho3D::XMLFile* style, const String& styleName) {
	MySprite* sprite = new MySprite(Game::get()->getContext());

	sprite->SetStyle(styleName, style);
	return sprite;
}

static void setTextureToSprite(MySprite* sprite, Texture2D* texture) {
	sprite->SetTexture(texture);
	const int textureWidth = texture->GetWidth();
	const int textureHeight = texture->GetHeight();
	//IntVector2 size = sprite->GetSize();
	IntVector2 size = sprite->getMySize();
	const float scaleX = size.x_ / (float)textureWidth;
	const float scaleY = size.y_ / (float)textureHeight;
	sprite->SetScale(1);
	if (scaleX < scaleY) {
		sprite->SetScale(scaleX);
	} else {
		sprite->SetScale(scaleY);
	}

	sprite->SetSize(textureWidth, textureHeight);
	Vector2 perHotSpot = sprite->getPercentHotSpot();
	sprite->SetFullImageRect();
	sprite->SetHotSpot(textureWidth * perHotSpot.x_, textureHeight * perHotSpot.y_);
}

static void setExactTextureToSprite(MySprite* sprite, Texture2D* texture) {
	sprite->SetTexture(texture);
	const int textureWidth = texture->GetWidth();
	const int textureHeight = texture->GetHeight();

	sprite->SetSize(textureWidth, textureHeight);
	Vector2 perHotSpot = sprite->getPercentHotSpot();
	sprite->SetFullImageRect();
	sprite->SetHotSpot(textureWidth * perHotSpot.x_, textureHeight * perHotSpot.y_);
}


static MySprite* createSprite(Texture2D* texture, Urho3D::XMLFile* style, const String& styleName) {
	MySprite* sprite = createEmptySprite(style, styleName);
	setTextureToSprite(sprite, texture);

	return sprite;
}

inline void addTextItem(Urho3D::DropDownList* cob, Urho3D::String str, Urho3D::XMLFile* style) {
	Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
	item->SetStyle("MyText", style);
	item->SetText(str);
	cob->AddItem(item);
}

inline Urho3D::DropDownList*
createDropDownList(Urho3D::UIElement* uiElement, String styleName, Urho3D::XMLFile* style) {
	Urho3D::DropDownList* cob = uiElement->CreateChild<Urho3D::DropDownList>();
	cob->SetStyle(styleName, style);
	return cob;
}

inline void addChildText(UIElement* element, String styleName, String value, Urho3D::XMLFile* style) {
	Urho3D::Text* text = element->CreateChild<Urho3D::Text>();
	text->SetStyle(styleName, style);
	text->SetText(value);
}
