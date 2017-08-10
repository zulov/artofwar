#pragma once
#include <Urho3D/UI/Button.h>
#include "ObjectEnums.h"
#include "Game.h"
#include "SelectedInfo.h"
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/Texture2D.h>
#include "SelectedHudElement.h"

class SelectedHudPanel
{
public:
	SelectedHudPanel(Urho3D::XMLFile* _style, Window* _window);
	~SelectedHudPanel();	
	void updateSelected(SelectedInfo* selectedInfo);
	void hide();
	std::vector<Button*>* getButtonsSelectedToSubscribe();

private:
	short MAX_ICON_SELECTION = 33;
	short LINES_IN_SELECTION = 4;
	String getName(ObjectType index, int i);
	int getSize(ObjectType type);
	std::vector<Button*>* buttons;
	SelectedHudElement** elements;
	Urho3D::XMLFile* style;
	Window* window;
	UIElement **test;
};

