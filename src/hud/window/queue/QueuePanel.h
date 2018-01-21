#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include <vector>
#include "QueueHudElement.h"
#include "objects/building/QueueManager.h"
#include "control/SelectedInfo.h"

class QueuePanel :public AbstractWindowPanel//TODO moze zrobic multilina
{
public:
	QueuePanel(Urho3D::XMLFile* _style);
	~QueuePanel();

	static String getIconName(ObjectType index, int id);
	void show(SelectedInfo* selectedInfo);
	void update(SelectedInfo* selectedInfo);
private:
	void update(QueueManager* queue, short& j);
	void hideElements(int from);
	void createBody() override;
	void HandleReduce(StringHash eventType, VariantMap& eventData);
	short MAX_ICON_SELECTION = 31;

	QueueHudElement** elements;
};
