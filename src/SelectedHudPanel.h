#pragma once
#include <Urho3D/UI/Button.h>
#include "ObjectEnums.h"
#include "Game.h"
#include <Urho3D/UI/Font.h>
#include "SelectedInfo.h"
#include <Urho3D/UI/Window.h>
#include "SelectedInfo.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Sprite.h>
#include "ButtonUtils.h"

#define MAX_ICON_SELECTION 20

class SelectedHudPanel
{
public:
	void initButtons(ObjectType type, int size);
	SelectedHudPanel(Urho3D::XMLFile* _style, Window* _window);
	~SelectedHudPanel();
	
	void updateSelected(SelectedInfo* selectedInfo);
	void hide();
private:
	String getName(ObjectType index, int i);
	int getSize(ObjectType type);
	void createButton(int size, ObjectType index);
	Urho3D::Button**** buttons;
	Urho3D::XMLFile* style;
	Window* window;
};

