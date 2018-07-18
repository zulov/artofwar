#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>


class SelectedInfo;
class SelectedHudElement;

class SelectedHudPanel :public AbstractWindowPanel
{
public:
	explicit SelectedHudPanel(Urho3D::XMLFile* _style);
	~SelectedHudPanel();	
	void update(SelectedInfo* selectedInfo);
	
	std::vector<Urho3D::Button*>& getButtonsSelectedToSubscribe();
	void createRows();

private:
	int iconSize();
	void hide(int i);
	void createBody() override;
	short maxInRow = 31;
	short LINES_IN_SELECTION = 4;
	std::vector<Urho3D::Button*> buttons;
	SelectedHudElement** elements;
	Urho3D::UIElement **rows;
};

