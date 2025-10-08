#include "QueuePanel.h"
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/ui/button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "GameState.h"
#include "QueueHudElement.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "objects/NamesCache.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueElement.h"


QueuePanel::QueuePanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : EventPanel(root, _style, "QueueWindow",
                                                                      {GameState::RUNNING, GameState::PAUSE}) {
	elements = new QueueHudElement*[MAX_ICON_SELECTION];
}

QueuePanel::~QueuePanel() {
	clear_array(elements, MAX_ICON_SELECTION);

	delete[] elements;
}

void QueuePanel::update(const QueueManager& queue, short& j) const {
	const short size = Urho3D::Min(queue.getSize(), MAX_ICON_SELECTION);
	for (int i = 0; i < size; ++i) {
		QueueElement* element = queue.getAt(i);
		elements[j]->show();
		auto name = getIconName(element->getType(), element->getAmount(), element->getId());
		auto texture = getTexture("textures/hud/icon/" + name);
		if (element->getMaxCapacity() > 1) {
			elements[j]->setText(Urho3D::String(element->getAmount()) + "/" + Urho3D::String(element->getMaxCapacity()));
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

void QueuePanel::show(const QueueManager& queue) {
	short j = 0;
	update(queue, j);
	finish(j);
}

void QueuePanel::update(SelectedInfo* selectedInfo) {
	//TODO wykonuje sie nawet jeœli sie nic nie zmieni³o
	short j = 0;
	if (window->IsVisible()) {
		for (auto infoType : selectedInfo->getSelectedTypes()) {
			for (Physical* physical : infoType->getData()) {
				//TODO przeniesc kolejke do Physical
				const auto building = dynamic_cast<Building*>(physical);
				update(building->getQueue(), j);
			}
		}
	}
	finish(j);
}

void QueuePanel::finish(int from) {
	for (int i = from; i < MAX_ICON_SELECTION; ++i) {
		elements[i]->hide();
	}
	if (from == 0) {
		setVisible(false);
	} else {
		setVisible(true);
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
