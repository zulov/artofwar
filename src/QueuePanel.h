#pragma once
#include "AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include <vector>
#include "QueueHudElement.h"
#include "QueueManager.h"
#include "SelectedInfo.h"
#include <Urho3D/Graphics/Texture2D.h>

class QueuePanel :public AbstractWindowPanel//TODO moze zrobic multilina
{
public:
	QueuePanel(Urho3D::XMLFile* _style);
	~QueuePanel();

	String getIconName(ObjectType index, int id);
	std::vector<Button*>* getButtonsSelectedToSubscribe();
	void show(SelectedInfo* selectedInfo);
private:
	void update(QueueManager* queue, short& j);
	void hide();
	void createBody() override;
	short MAX_ICON_SELECTION = 31;

	std::vector<Button*>* buttons;
	QueueHudElement** elements;
};
