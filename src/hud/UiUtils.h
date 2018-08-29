#pragma once
#include "Game.h"
#include "MySprite.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <vector>

static Urho3D::Button* setStyle(MySprite* sprite, Urho3D::XMLFile* style, const Urho3D::String& styleName,
                                Urho3D::Button* button) {
	button->SetStyle(styleName, style);
	if (sprite) {
		button->AddChild(sprite);
	}
	return button;
}

static Urho3D::Button* simpleButton(MySprite* sprite, Urho3D::XMLFile* style, const Urho3D::String& styleName) {
	auto button = new Urho3D::Button(Game::getContext());
	return setStyle(sprite, style, styleName, button);
}

static Urho3D::Button* simpleButton(Urho3D::UIElement* parent, MySprite* sprite, Urho3D::XMLFile* style,
                                    const Urho3D::String& styleName) {
	auto button = parent->CreateChild<Urho3D::Button>();
	return setStyle(sprite, style, styleName, button);
}

static MySprite* createEmptySprite(Urho3D::XMLFile* style, const Urho3D::String& styleName) {
	auto sprite = new MySprite(Game::getContext());

	sprite->SetStyle(styleName, style);
	return sprite;
}

static void setTextureToSprite(MySprite* sprite, Urho3D::Texture2D* texture) {
	if (texture) {
		sprite->SetVisible(true);
		sprite->SetTexture(texture);
		const int textureWidth = texture->GetWidth();
		const int textureHeight = texture->GetHeight();
		//IntVector2 size = sprite->GetSize();
		auto size = sprite->getMySize();
		const float scaleX = size.x_ / (float)textureWidth;
		const float scaleY = size.y_ / (float)textureHeight;
		sprite->SetScale(1);
		if (scaleX < scaleY) {
			sprite->SetScale(scaleX);
		} else {
			sprite->SetScale(scaleY);
		}

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


static MySprite* createSprite(Urho3D::Texture2D* texture, Urho3D::XMLFile* style, const Urho3D::String& styleName) {
	auto sprite = createEmptySprite(style, styleName);
	setTextureToSprite(sprite, texture);

	return sprite;
}

inline void addTextItem(Urho3D::DropDownList* cob, Urho3D::String str, Urho3D::XMLFile* style) {
	auto item = new Urho3D::Text(Game::getContext());
	item->SetStyle("MyText", style);
	item->SetText(str);
	cob->AddItem(item);
}

inline Urho3D::DropDownList*
createDropDownList(Urho3D::UIElement* uiElement, Urho3D::String styleName, Urho3D::XMLFile* style) {
	auto cob = uiElement->CreateChild<Urho3D::DropDownList>();
	cob->SetStyle(styleName, style);
	return cob;
}

inline Urho3D::Text* addChildText(Urho3D::UIElement* element, Urho3D::String styleName, Urho3D::String value,
                         Urho3D::XMLFile* style) {
	auto text = element->CreateChild<Urho3D::Text>();
	text->SetStyle(styleName, style);
	text->SetText(value);
	return text;
}

inline void addChildTexts(Urho3D::DropDownList* cob, std::vector<Urho3D::String> names, Urho3D::XMLFile* style) {
	for (auto& name : names) {
		addTextItem(cob, name, style);
	}
}

inline void addTextItem(Urho3D::DropDownList* cob, Urho3D::String& name, Urho3D::XMLFile* style, Urho3D::Variant var,
                        const Urho3D::String& varName) {
	auto item = new Urho3D::Text(Game::getContext());
	item->SetStyle("MyText", style);
	item->SetText(name);
	cob->AddItem(item);
	item->SetVar(varName, var);
}

inline void addChildTexts(Urho3D::DropDownList* cob, std::vector<Urho3D::String> names, Urho3D::XMLFile* style,
                          std::vector<Urho3D::Variant> vars, Urho3D::String varsName) {
	for (int i = 0; i < names.size(); ++i) {
		addTextItem(cob, names.at(i), style, vars.at(i), varsName);
	}
}
