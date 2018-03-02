#pragma once
#include "ObjectEnums.h"
#include "objects/queue/QueueType.h"
#include <Urho3D/UI/UIElement.h>


class HudData
{
public:
	HudData(Urho3D::UIElement * _uiElement);
	~HudData();
	Urho3D::UIElement * getUIElement();

	void setId(short id, ObjectType _type);
	void setId(short id, ObjectType _type, QueueType _queueType);
	short getId();
	ObjectType getType();
	QueueType getQueueType();
private:
	Urho3D::UIElement * uiElement;
	ObjectType type;
	QueueType queueType;
	
	short objectId;

};

