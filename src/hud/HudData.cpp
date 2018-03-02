#include "HudData.h"


HudData::HudData(Urho3D::UIElement* _uiElement) {
	uiElement = _uiElement;
}

HudData::~HudData() = default;

Urho3D::UIElement* HudData::getUIElement() {
	return uiElement;
}

void HudData::setId(short id, ObjectType _type) {
	objectId = id;
	type = _type;
}

void HudData::setId(short id, ObjectType _type, QueueType _queueType) {
	objectId = id;
	type = _type;
	queueType = _queueType;
}

short HudData::getId() {
	return objectId;
}

ObjectType HudData::getType() {
	return type;
}

QueueType HudData::getQueueType() {
	return queueType;
}
