#include "AbstractMiddlePanel.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


AbstractMiddlePanel::
AbstractMiddlePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title):
	EventPanel(_style, "AdditionalInGameWindow", {}) {
	msg = _title;
}


AbstractMiddlePanel::~AbstractMiddlePanel() = default;

void AbstractMiddlePanel::createBody() {
	buttonClose = createElement<Urho3D::Button>(window, style, "MyCloseButton");

	SubscribeToEvent(buttonClose, Urho3D::E_CLICK, URHO3D_HANDLER(AbstractMiddlePanel, HandleClose));
	title = addChildText(window, "MiddleTitle", msg, style);

	body = createElement<Urho3D::UIElement>(window, style, "MiddleMock");
}

void AbstractMiddlePanel::HandleClose(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	setVisible(false);
}

Urho3D::Button* AbstractMiddlePanel::getMainButton() {
	return nullptr;
}
