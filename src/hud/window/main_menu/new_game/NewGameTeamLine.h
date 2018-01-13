#pragma once
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/DropDownList.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include <Urho3D/UI/Text.h>

struct NewGameTeamLine
{
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* nation;
	Urho3D::DropDownList* color;

	void populateTeams(Urho3D::Localization* l10n, Urho3D::BorderImage* row, Urho3D::XMLFile* style) {

		lineEdit = row->CreateChild<Urho3D::LineEdit>();
		lineEdit->SetStyle("LineEdit", style);

		nation = row->CreateChild<Urho3D::DropDownList>();
		nation->SetStyle("MainMenuNewGameDropDownList", style);
		int size = Game::get()->getDatabaseCache()->getNationSize();
		for (int i = 0; i < size; ++i) {
			Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
			item->SetStyle("MyText", style);
			item->SetText(
			              l10n->Get(
			                        "nation_" + Game::get()->getDatabaseCache()->getNation(i)->name
			                       ));
			nation->AddItem(item);
		}
		color = row->CreateChild<Urho3D::DropDownList>();
		color->SetStyle("MainMenuNewGameDropDownList", style);
		int sizeColor = Game::get()->getDatabaseCache()->getPlayerColorsSize();
		for (int i = 0; i < sizeColor; ++i) {
			Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
			item->SetStyle("MyText", style);
			item->SetText(
			              l10n->Get(
			                        "color_" + Game::get()->getDatabaseCache()->getPlayerColor(i)->name
			                       ));

			color->AddItem(item);
		}
	}
};
