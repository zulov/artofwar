#include "AbstractMiddlePanel.h"
#include <Urho3D/UI/UIEvents.h>


AbstractMiddlePanel::AbstractMiddlePanel(Urho3D::String _title): AbstractWindowPanel() {
	styleName = "AdditionalInGameWindow";
	msg = _title;
}


AbstractMiddlePanel::~AbstractMiddlePanel() = default;

void AbstractMiddlePanel::createBody() {
	buttonClose = window->CreateChild<Urho3D::Button>();
	buttonClose->SetStyle("MyCloseButton");
	SubscribeToEvent(buttonClose, Urho3D::E_CLICK, URHO3D_HANDLER(AbstractMiddlePanel, HandleClose));
	title = window->CreateChild<Urho3D::Text>();
	title->SetStyle("MiddleTitle");
	title->SetText(msg);

	body = window->CreateChild<Urho3D::UIElement>();
	body->SetStyle("MiddleMock");
}

void AbstractMiddlePanel::HandleClose(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	setVisible(false);
}

Urho3D::Button* AbstractMiddlePanel::getMainButton() {
	return nullptr;
}
