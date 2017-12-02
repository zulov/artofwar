#include "AbstractMiddlePanel.h"
#include <Urho3D/UI/UIEvents.h>


AbstractMiddlePanel::AbstractMiddlePanel(Urho3D::XMLFile* _style, Urho3D::String _title): AbstractWindowPanel(_style) {
	styleName = "AdditionalInGameWindow";
	msg = _title;
}


AbstractMiddlePanel::~AbstractMiddlePanel() {
}


void AbstractMiddlePanel::createBody() {
	buttonClose = window->CreateChild<Urho3D::Button>();
	buttonClose->SetStyle("MyCloseButton", style);
	SubscribeToEvent(buttonClose, Urho3D::E_CLICK, URHO3D_HANDLER(AbstractMiddlePanel, HandleClose));
	title = window->CreateChild<Urho3D::Text>();
	title->SetStyle("MiddleTitle", style);
	title->SetText(msg);

	body = window->CreateChild<Urho3D::UIElement>();
	body->SetStyle("MiddleMock", style);
}

void AbstractMiddlePanel::HandleClose(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	setVisible(false);
}
