#include "MenuPanel.h"
#include "Game.h"


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
}


MenuPanel::~MenuPanel() {
}

std::vector<HudElement*>* MenuPanel::getLists() {
	return lists;
}

void MenuPanel::createBody() {
	lists = new std::vector<HudElement*>();
	Urho3D::DropDownList* dropDownList = window->CreateChild<Urho3D::DropDownList>();
	dropDownList->SetStyle("MyDropDown", style);
	std::array<Urho3D::String, 3> modes{{"Select","Build","Deploy"}};
	for (String mode : modes) {
		Text* text = new Text(Game::get()->getContext());
		text->SetText(mode);
		text->SetStyle("MyText", style);
		dropDownList->AddItem(text);
	}

	window->AddChild(dropDownList);

	HudElement* hudElement = new HudElement(dropDownList);
	dropDownList->SetVar("HudElement", hudElement);
	lists->push_back(hudElement);

	text = window->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText("");
}
