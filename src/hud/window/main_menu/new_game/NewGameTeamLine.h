#pragma once
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include "NewGamePlayer.h"
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
#include "hud/UiUtils.h"
#include "math/RandGen.h"

namespace Urho3D { class BorderImage; }
namespace Urho3D { class Localization; }
namespace Urho3D { class XMLFile; }


struct NewGameTeamLine {
	NewGameTeamLine() = default;
	NewGameTeamLine(const NewGameTeamLine&) = delete;
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* nation;
	Urho3D::DropDownList* color;
	Urho3D::DropDownList* team;
	Urho3D::CheckBox* chk;

	Urho3D::XMLFile* style;
	Urho3D::Localization* l10n;
	int id;

	void init(Urho3D::XMLFile* style, Urho3D::Localization* l10n, int id) {
		this->style = style;
		this->l10n = l10n;
		this->id = id;
	}

	void populateTeams(Urho3D::BorderImage* row) {
		lineEdit = createElement<Urho3D::LineEdit>(row, style, "LineEdit");

		auto names = Game::getCache()->GetResource<Urho3D::JSONFile>("lang/names.json");
		auto namesArray = names->GetRoot().Get("player_names").GetArray();
		lineEdit->SetText(namesArray.At(RandGen::nextRand(RandIntType::PLAYER_NAME, namesArray.Size())).GetCString());

		team = createElement<Urho3D::DropDownList>(row, style, "MainMenuNewGameDropDownList");
		addChildTexts(team, {l10n->Get("1"), l10n->Get("2")}, style);

		nation = createElement<Urho3D::DropDownList>(row, style, "MainMenuNewGameDropDownList");


		for (auto dbNation : Game::getDatabase()->getNations()) {
			if (dbNation) {
				addTextItem(nation, l10n->Get("nation_" + dbNation->name), style);
			}
		}

		color = createElement<Urho3D::DropDownList>(row, style, "MainMenuNewGameDropDownList");

		for (auto playerColor : Game::getDatabase()->getPlayerColors()) {
			if (playerColor) {
				addTextItem(color, l10n->Get("color_" + playerColor->name), style);
			}
		}
		chk = createElement<Urho3D::CheckBox>(row, style, "CheckBox");
	}

	NewGamePlayer getNewGamePlayer() const {
		NewGamePlayer player;
		player.name = lineEdit->GetText();
		player.color = color->GetSelection();
		player.nation = nation->GetSelection();
		player.team = team->GetSelection();
		player.id = id;
		player.active = chk->IsChecked();
		return player;
	}

	Urho3D::CheckBox* getCheckBox() const {
		return chk;
	}

	void setCheck(bool check) const {
		chk->SetChecked(check);
	}
};
