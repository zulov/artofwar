#include "QueuePanel.h"
#include "Game.h"
#include "Building.h"


QueuePanel::QueuePanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "QueueWindow";
}


QueuePanel::~QueuePanel() {
}

void QueuePanel::update(QueueManager* queue, short& j) {
	short size = queue->getSize();
	for (int i = 0; i < size; ++i) {
		QueueElement* element = queue->getAt(i);
		elements[j]->show();
		String name = getIconName(element->getType(), element->getSubtype());
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + name);
		elements[j]->setText(String(element->getAmount()) + "/" + String(element->getMaxCapacity()));
		elements[j]->setTexture(texture);
		elements[j]->setProgress(element->getProgress());
		++j;
	}
}

String QueuePanel::getIconName(ObjectType index, int id) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (index) {
	case ENTITY:
		return "mock.png";
	case UNIT:
		return dbCache->getUnit(id)->icon;
	case BUILDING:
		return dbCache->getBuilding(id)->icon;
	case RESOURCE:
		return dbCache->getResource(id)->icon;
	default:
		return "mock.png";
	}
}

std::vector<Button*>* QueuePanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void QueuePanel::show(SelectedInfo* selectedInfo) {//TODO tu moga byc problemy przy duzej ilosci
	setVisible(true);
	update(selectedInfo);
}

void QueuePanel::update(SelectedInfo* selectedInfo) {
	short j = 0;
	if (window->IsVisible()) {
		vector<SelectedInfoType*>* infoTypes = selectedInfo->getSelecteType();

		for (int i = 0; i < infoTypes->size(); ++i) {
			std::vector<Physical*>* data = infoTypes->at(i)->getData();
			for (Physical* physical : (*data)) {//TODO przeniesc kolejke do Physical
				Building* building = static_cast<Building*>(physical);
				update(building->getQueue(), j);
			}
		}
	}
	hideElements(j);
}

void QueuePanel::hideElements(int from) {
	for (int i = from; i < MAX_ICON_SELECTION; ++i) {
		elements[i]->hide();
	}
}

void QueuePanel::createBody() {
	elements = new QueueHudElement*[MAX_ICON_SELECTION];
	buttons = new std::vector<Button*>();
	buttons->reserve(MAX_ICON_SELECTION);
	for (int i = 0; i < MAX_ICON_SELECTION; ++i) {
		elements[i] = new QueueHudElement(style);
		window->AddChild(elements[i]->getButton());
		buttons->push_back(elements[i]->getButton());
	}
}
