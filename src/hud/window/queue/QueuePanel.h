#pragma once
#include "../AbstractWindowPanel.h"

class SelectedInfo;
class QueueManager;
class QueueHudElement;

class QueuePanel :public AbstractWindowPanel//TODO moze zrobic multilina
{
public:
	explicit QueuePanel(Urho3D::XMLFile* _style);
	~QueuePanel();
	void update(QueueManager* queue, short& j) const;

	void show(SelectedInfo* selectedInfo);
	void show(QueueManager* queue);
	void update(SelectedInfo* selectedInfo) const;
	void update(QueueManager* queue) const;
private:
	void hideElements(int from) const;
	void createBody() override;
	void HandleReduce(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	char MAX_ICON_SELECTION = 31;

	QueueHudElement** elements;
};
