#pragma once
#include "hud/window/EventPanel.h"

class AbstractQueueManager;
class SelectedInfo;
class QueueManager;
class QueueHudElement;

class QueuePanel : public EventPanel {
public:
	explicit QueuePanel(Urho3D::XMLFile* _style);
	~QueuePanel();

	void show(SelectedInfo* selectedInfo);
	void show(AbstractQueueManager* queue);
	void update(SelectedInfo* selectedInfo);
private:
	void finish(int from);
	void update(AbstractQueueManager* queue, short& j) const;
	void createBody() override;
	void HandleReduce(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	char MAX_ICON_SELECTION = 31;

	QueueHudElement** elements;
};
