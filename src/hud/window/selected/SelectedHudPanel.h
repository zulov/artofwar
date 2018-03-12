#pragma once
#include "../AbstractWindowPanel.h"
#include "Game.h"
#include "SelectedHudElement.h"
#include "control/SelectedInfo.h"
#include <Urho3D/UI/Button.h>


class SelectedHudPanel :public AbstractWindowPanel
{
public:
	SelectedHudPanel(Urho3D::XMLFile* _style);
	~SelectedHudPanel();	
	void update(SelectedInfo* selectedInfo);
	
	std::vector<Button*>& getButtonsSelectedToSubscribe();
	int iconSize();
	void createRows();

private:
	void hide(int i);
	void createBody() override;
	short maxInRow = 31;
	short LINES_IN_SELECTION = 4;
	std::vector<Button*> buttons;
	SelectedHudElement** elements;
	UIElement **rows;
};

