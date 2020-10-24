#pragma once
#include <utility>
#include <vector>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include "Game.h"
#include "MySprite.h"

static Urho3D::Button* simpleButton(Urho3D::XMLFile* style, const Urho3D::String& styleName) {
	auto button = new Urho3D::Button(Game::getContext());
	button->SetStyle(styleName, style);
	return button;
}

static void setTextureToSprite(MySprite* sprite, Urho3D::Texture2D* texture) {
	if (texture) {
		sprite->SetVisible(true);
		sprite->SetTexture(texture);
		const int textureWidth = texture->GetWidth();
		const int textureHeight = texture->GetHeight();

		const auto size = sprite->getMySize();
		const float scaleX = size.x_ / (float)textureWidth;
		const float scaleY = size.y_ / (float)textureHeight;

		sprite->SetScale(Urho3D::Min(scaleX, scaleY));

		sprite->SetSize(textureWidth, textureHeight);
		const auto perHotSpot = sprite->getPercentHotSpot();
		sprite->SetFullImageRect();
		sprite->SetHotSpot(textureWidth * perHotSpot.x_, textureHeight * perHotSpot.y_);
	} else {
		sprite->SetVisible(false);
	}
}

static void setExactTextureToSprite(MySprite* sprite, Urho3D::Texture2D* texture) {
	sprite->SetTexture(texture);
	const int textureWidth = texture->GetWidth();
	const int textureHeight = texture->GetHeight();

	sprite->SetSize(textureWidth, textureHeight);
	const auto perHotSpot = sprite->getPercentHotSpot();
	sprite->SetFullImageRect();
	sprite->SetHotSpot(textureWidth * perHotSpot.x_, textureHeight * perHotSpot.y_);
}

template <class T>
T* createElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style,
                 const Urho3D::String& styleName) {
	auto element = parent->CreateChild<T>();
	element->SetStyle(styleName, style);
	return element;
}

static MySprite* createSprite(Urho3D::UIElement* parent, Urho3D::Texture2D* texture, Urho3D::XMLFile* style,
                              const Urho3D::String& styleName) {
	const auto sprite = createElement<MySprite>(parent, style, styleName);
	setTextureToSprite(sprite, texture);

	return sprite;
}

inline Urho3D::Text* addTextItem(Urho3D::DropDownList* cob, const Urho3D::String& str, Urho3D::XMLFile* style) {
	auto item = new Urho3D::Text(Game::getContext());
	item->SetStyle("MyText", style);
	item->SetText(str);
	cob->AddItem(item);
	return item;
}

inline Urho3D::Text* addChildText(Urho3D::UIElement* parent, const Urho3D::String& styleName, Urho3D::XMLFile* style) {
	return createElement<Urho3D::Text>(parent, style, styleName);
}

inline Urho3D::Text* addChildText(Urho3D::UIElement* parent, Urho3D::String styleName, const Urho3D::String& value,
                                  Urho3D::XMLFile* style) {
	auto text = addChildText(parent, std::move(styleName), style);
	text->SetText(value);
	return text;
}


inline void addChildTexts(Urho3D::DropDownList* cob, const std::vector<Urho3D::String>& names, Urho3D::XMLFile* style) {
	for (auto& name : names) {
		addTextItem(cob, name, style);
	}
}

inline void addTextItem(Urho3D::DropDownList* cob, Urho3D::String& name, Urho3D::XMLFile* style,
                        const Urho3D::Variant& var, const Urho3D::String& varName) {
	addTextItem(cob, name, style)->SetVar(varName, var);
}

inline void addChildTexts(Urho3D::DropDownList* cob, std::vector<Urho3D::String> names, Urho3D::XMLFile* style,
                          std::vector<Urho3D::Variant> vars, const Urho3D::String& varsName) {
	for (int i = 0; i < names.size(); ++i) {
		addTextItem(cob, names.at(i), style, vars.at(i), varsName);
	}
}
