#include "MainMenuClosePanel.h"

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


MainMenuClosePanel::
MainMenuClosePanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, const Urho3D::String& _title): MainMenuDetailsPanel(root, _style, _title) {
	bodyStyle = "MainMenuCloseMock";
}

Urho3D::Button* MainMenuClosePanel::getCloseButton() const {
	return yes;
}

void MainMenuClosePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	auto l10n = Game::getLocalization();
	miniWindow = createElement<Urho3D::Window>(body, style, "MiniWindow");
	miniWindow->SetVisible(true);

	text = addChildText(miniWindow, "CloseText", l10n->Get("if_close"), style);

	row = createElement<Urho3D::UIElement>(miniWindow, style, "MainMenuCloseRow");

	yes = createElement<Urho3D::Button>(row, style, "MsgButton");
	addChildText(yes, "MsgTextButton", l10n->Get("quit"), style);

	no = createElement<Urho3D::Button>(row, style, "MsgButton");
	addChildText(no, "MsgTextButton", l10n->Get("cancel"), style);

	SubscribeToEvent(no, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuClosePanel, HandleCancelClick));
}

void MainMenuClosePanel::HandleCancelClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	setVisible(false);
}
