#pragma once
#include "../AbstractWindowPanel.h"

namespace Urho3D {
	class Button;
	class UIElement;
	class XMLFile;
}

class SelectedInfo;
class SelectedHudElement;

class SelectedHudPanel : public AbstractWindowPanel {
public:
	explicit SelectedHudPanel(Urho3D::XMLFile* _style);
	~SelectedHudPanel();
	void update(SelectedInfo* selectedInfo);

	std::vector<Urho3D::Button*> getButtonsSelectedToSubscribe() const;
	void createRows();
	void clearSelected();

private:
	int iconSize() const;
	void hide(int i);
	void createBody() override;
	char maxInRow;
	const char linesNumber = 3;
	char lastHidden;

	SelectedHudElement** elements;
	Urho3D::UIElement** rows;
};
