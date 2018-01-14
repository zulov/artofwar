#pragma once
#include "Game.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include "NewGamePlayer.h"


struct NewGameTeamLine
{
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* nation;
	Urho3D::DropDownList* color;
	Urho3D::DropDownList* team;

	Urho3D::XMLFile* style;
	Urho3D::Localization* l10n;


	void init(Urho3D::XMLFile* style, Urho3D::Localization* l10n) {
		this->style = style;
		this->l10n = l10n;
	}

	void populateTeams(Urho3D::BorderImage* row) {
		lineEdit = row->CreateChild<Urho3D::LineEdit>();
		lineEdit->SetStyle("LineEdit", style);

		Urho3D::JSONFile* names = Game::get()->getCache()->GetResource<Urho3D::JSONFile>("lang/names.json");
		Urho3D::JSONArray namesArray = names->GetRoot().Get("player_names").GetArray();
		lineEdit->SetText(namesArray.At(rand() % namesArray.Size()).GetCString());

		team = createDropDownList(row, "MainMenuNewGameDropDownList", style);
		addTextItem(team, "1", style);
		addTextItem(team, "2", style);

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
	}

	NewGamePlayer getNewGamePlayer() {
		NewGamePlayer player;
		player.name = lineEdit->GetText();
		player.color = color->GetSelection();
		player.nation = nation->GetSelection();
		player.team = team->GetSelection();
		return player;
	}
};
