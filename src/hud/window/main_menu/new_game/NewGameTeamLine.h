#pragma once
#include "Game.h"
#include "NewGamePlayer.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>


struct NewGameTeamLine
{
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* nation;
	Urho3D::DropDownList* color;
	Urho3D::DropDownList* team;
	Urho3D::CheckBox* chk;

	Urho3D::XMLFile* style;
	Urho3D::Localization* l10n;
	int id;

	void init(Urho3D::XMLFile* style, Urho3D::Localization* l10n, int _id) {
		this->style = style;
		this->l10n = l10n;
		id = _id;
	}

	void populateTeams(Urho3D::BorderImage* row) {
		lineEdit = row->CreateChild<Urho3D::LineEdit>();
		lineEdit->SetStyle("LineEdit", style);

		Urho3D::JSONFile* names = Game::get()->getCache()->GetResource<Urho3D::JSONFile>("lang/names.json");
		Urho3D::JSONArray namesArray = names->GetRoot().Get("player_names").GetArray();
		lineEdit->SetText(namesArray.At(rand() % namesArray.Size()).GetCString());

		team = createDropDownList(row, "MainMenuNewGameDropDownList", style);
			addChildTexts(team, {l10n->Get("1"), l10n->Get("2")}, style);

		nation = createDropDownList(row, "MainMenuNewGameDropDownList", style);
		int size = Game::get()->getDatabaseCache()->getNationSize();
		for (int i = 0; i < size; ++i) {
			addTextItem(nation, l10n->Get("nation_" + Game::get()->getDatabaseCache()->getNation(i)->name), style);
		}

		color = createDropDownList(row, "MainMenuNewGameDropDownList", style);
		int sizeColor = Game::get()->getDatabaseCache()->getPlayerColorsSize();
		for (int i = 0; i < sizeColor; ++i) {
			addTextItem(color, l10n->Get("color_" + Game::get()->getDatabaseCache()->getPlayerColor(i)->name), style);
		}
		chk = row->CreateChild<CheckBox>();
		chk->SetStyle("CheckBox", style);
	}

	NewGamePlayer getNewGamePlayer() {
		NewGamePlayer player;
		player.name = lineEdit->GetText();
		player.color = color->GetSelection();
		player.nation = nation->GetSelection();
		player.team = team->GetSelection();
		player.id = id;
		player.active = chk->IsChecked();
		return player;
	}

	CheckBox* getCheckBox() {
		return chk;
	}

	void setCheck(bool check) {
		chk->SetChecked(check);
	}
};
