#include "QueuePanel.h"
#include "GameState.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "database/DatabaseCache.h"
#include "objects/NamesCache.h"
#include "objects/building/Building.h"
#include "control/SelectedInfoType.h"
#include "objects/queue/QueueElement.h"
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UIEvents.h>
#include "QueueHudElement.h"
#include "control/SelectedInfo.h"


QueuePanel::QueuePanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "QueueWindow",
                                                                      {GameState::RUNNING, GameState::PAUSE}) {
	elements = new QueueHudElement*[MAX_ICON_SELECTION];
}

QueuePanel::~QueuePanel() {
	for (int i = 0; i < MAX_ICON_SELECTION; ++i) {
		delete elements[i];
	}
	delete[] elements;
}

void QueuePanel::update(QueueManager& queue, short& j) const {
	const short size = queue.getSize();
	for (int i = 0; i < size; ++i) {
		QueueElement* element = queue.getAt(i);
		elements[j]->show();
		auto name = getIconName(element->getType(), element->getAmount(), element->getId());
		auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/" + name);
		if (element->getMaxCapacity() > 1) {
			elements[j]->
				setText(Urho3D::String(element->getAmount()) + "/" + Urho3D::String(element->getMaxCapacity()));
		} else {
			elements[j]->hideText();
		}
		elements[j]->setTexture(texture);
		elements[j]->setProgress(element->getProgress());
		elements[j]->setData(element);

		++j;
	}
}

void QueuePanel::show(SelectedInfo* selectedInfo) {
	//TODO tu moga byc problemy przy duzej ilosci
	setVisible(true);
	update(selectedInfo);
}

void QueuePanel::update(QueueManager& queue) const {
	short j = 0;
	update(queue, j);
	hideElements(j);
}

void QueuePanel::show(QueueManager& queue) {
	setVisible(true);
	update(queue);
}

void QueuePanel::update(SelectedInfo* selectedInfo) const {
	//TODO wykonuje sie nawet jeœli sie nic nie zmieni³o
	short j = 0;
	if (window->IsVisible()) {
		std::vector<SelectedInfoType*> & infoTypes = selectedInfo->getSelectedTypes();

		for (auto& infoType : infoTypes) {
			std::vector<Physical*> & data = infoType->getData();
			for (Physical* physical : data) {
				//TODO przeniesc kolejke do Physical
				const auto building = dynamic_cast<Building*>(physical);
				update(*building->getQueue(), j);
			}
		}
	}
	hideElements(j);
}

void QueuePanel::hideElements(int from) const {
	for (int i = from; i < MAX_ICON_SELECTION; ++i) {
		elements[i]->hide();
	}
}

void QueuePanel::createBody() {
	for (int i = 0; i < MAX_ICON_SELECTION; ++i) {
		elements[i] = new QueueHudElement(window, style);
		SubscribeToEvent(elements[i]->getButton(), Urho3D::E_CLICK, URHO3D_HANDLER(QueuePanel, HandleReduce));
	}
}

void QueuePanel::HandleReduce(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto qHudElement = QueueHudElement::getFromElement(eventData);

	if (eventData[Urho3D::UIMouseClick::P_BUTTON].GetInt() == Urho3D::MOUSEB_LEFT) {
		qHudElement->reduce(1);
	} else {
		qHudElement->reduce(10);
	}
}
