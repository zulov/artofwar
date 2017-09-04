#pragma once
#include "AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include <vector>
#include "QueueHudElement.h"
#include "objects/building/QueueManager.h"
#include "control/SelectedInfo.h"
#include <Urho3D/Graphics/Texture2D.h>

class QueuePanel :public AbstractWindowPanel//TODO moze zrobic multilina
{
public:
	QueuePanel(Urho3D::XMLFile* _style);
	~QueuePanel();

	String getIconName(ObjectType index, int id);
	std::vector<Button*>* getButtonsSelectedToSubscribe();
	void show(SelectedInfo* selectedInfo);
	void update(SelectedInfo* selectedInfo);
private:
	void update(QueueManager* queue, short& j);
	void hideElements(int from);
	void createBody() override;
	short MAX_ICON_SELECTION = 31;

	std::vector<Button*>* buttons;
	QueueHudElement** elements;
};
