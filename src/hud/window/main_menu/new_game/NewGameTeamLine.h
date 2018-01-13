#pragma once
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/DropDownList.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>

struct NewGameTeamLine
{
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* nation;
	Urho3D::DropDownList* color;
	Urho3D::DropDownList* team;

	Urho3D::XMLFile* style;
	Urho3D::Localization* l10n;


	NewGameTeamLine() = default;

	void init(Urho3D::XMLFile* style, Urho3D::Localization* l10n) {
		this->style = style;
		this->l10n = l10n;
	}

	void addTextItem(Urho3D::DropDownList* cob, Urho3D::String str) {
		Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
		item->SetStyle("MyText", style);
		item->SetText(l10n->Get(str));
		cob->AddItem(item);
	}

	Urho3D::DropDownList* createDropDownList(Urho3D::BorderImage* row) {
		Urho3D::DropDownList* cob = row->CreateChild<Urho3D::DropDownList>();
		cob->SetStyle("MainMenuNewGameDropDownList", style);
		return cob;
	}

	void populateTeams(Urho3D::BorderImage* row) {
		lineEdit = row->CreateChild<Urho3D::LineEdit>();
		lineEdit->SetStyle("LineEdit", style);

		Urho3D::JSONFile* names = Game::get()->getCache()->GetResource<Urho3D::JSONFile>("lang/names.json");
		Urho3D::JSONArray namesArray = names->GetRoot().Get("player_names").GetArray();
		lineEdit->SetText(namesArray.At(rand() % namesArray.Size()).GetCString());

		team = createDropDownList(row);
		addTextItem(team, "1");
		addTextItem(team, "2");

		nation = createDropDownList(row);
		int size = Game::get()->getDatabaseCache()->getNationSize();
		for (int i = 0; i < size; ++i) {
			addTextItem(nation, "nation_" + Game::get()->getDatabaseCache()->getNation(i)->name);
		}

		color = createDropDownList(row);
		int sizeColor = Game::get()->getDatabaseCache()->getPlayerColorsSize();
		for (int i = 0; i < sizeColor; ++i) {
			addTextItem(color, "color_" + Game::get()->getDatabaseCache()->getPlayerColor(i)->name);
		}
	}
};
