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
	void update(SelectedInfo* selectedInfo) const;
	
	std::vector<Urho3D::Button*>& getButtonsSelectedToSubscribe();
	void createRows();
	void clearSelected() const;

private:
	int iconSize() const;
	void hide(int i) const;
	void createBody() override;
	char maxInRow;
	const char linesNumber = 3;
	std::vector<Urho3D::Button*> buttons;
	SelectedHudElement** elements;
	Urho3D::UIElement **rows;
};

