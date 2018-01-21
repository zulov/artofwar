#include "MainMenuClosePanel.h"
#include "Game.h"
#include <Urho3D/UI/Button.h>
#include "hud/UiUtils.h"
#include <Urho3D/UI/UIEvents.h>


MainMenuClosePanel::
MainMenuClosePanel( Urho3D::String _title): MainMenuDetailsPanel( _title) {
	bodyStyle = "MainMenuCloseMock";
}


MainMenuClosePanel::~MainMenuClosePanel() {
}

Urho3D::Button* MainMenuClosePanel::getCloseButton() {
	return yes;
}

void MainMenuClosePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	miniWindow = body->CreateChild<Urho3D::Window>();
	miniWindow->SetStyle("MiniWindow");
	miniWindow->SetVisible(true);

	text = miniWindow->CreateChild<Urho3D::Text>();
	text->SetStyle("CloseText");
	text->SetText(l10n->Get("if_close"));

	row = miniWindow->CreateChild<Urho3D::UIElement>();
	row->SetStyle("MainMenuCloseRow");

	yes = row->CreateChild<Urho3D::Button>();
	yes->SetStyle("MsgButton");
	addChildText(yes, "MsgTextButton", l10n->Get("quit"));

	no = row->CreateChild<Urho3D::Button>();
	no->SetStyle("MsgButton");
	addChildText(no, "MsgTextButton", l10n->Get("cancel"));

	SubscribeToEvent(no, E_CLICK, URHO3D_HANDLER(MainMenuClosePanel, HandleCancelClick));
}

void MainMenuClosePanel::HandleCancelClick(StringHash eventType, VariantMap& eventData) {
	setVisible(false);
}
