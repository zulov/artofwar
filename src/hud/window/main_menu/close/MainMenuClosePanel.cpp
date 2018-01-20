#include "MainMenuClosePanel.h"
#include "Game.h"
#include <Urho3D/UI/Button.h>
#include "hud/UiUtils.h"


MainMenuClosePanel::
MainMenuClosePanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuCloseMock";
}


MainMenuClosePanel::~MainMenuClosePanel() {
}

void MainMenuClosePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	miniWindow = body->CreateChild<Urho3D::Window>();
	miniWindow->SetStyle("MiniWindow", style);
	miniWindow->SetVisible(true);


	text = miniWindow->CreateChild<Urho3D::Text>();
	text->SetStyle("CloseText", style);
	text->SetText(l10n->Get("if_close"));

	row = miniWindow->CreateChild<Urho3D::UIElement>();
	row->SetStyle("MainMenuCloseRow", style);

	yes = row->CreateChild<Urho3D::Button>();
	yes->SetStyle("MsgButton", style);
	addChildText(yes, "MsgTextButton", l10n->Get("quit"), style);

	no = row->CreateChild<Urho3D::Button>();
	no->SetStyle("MsgButton", style);
	addChildText(no, "MsgTextButton", l10n->Get("cancel"), style);
}
