#include "TopInfoPanel.h"

#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/Localization.h>

#include "Game.h"
#include "hud/UiUtils.h"
#include "database/db_world_age_struct.h"
#include "player/Player.h"
#include "simulation/WorldAgeController.h"


TopInfoPanel::TopInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "TopInfoPanel", {}) {
}

void TopInfoPanel::hoverOn() {
	setVisible(true);
}

void TopInfoPanel::hoverOff() {
	setVisible(false);
}

void TopInfoPanel::update(const WorldAgeController* controller, const std::vector<Player*>& players) const {
	if (!controller) {
		return;
	}

	const auto* currentAge = controller->getAge(controller->getCurrentAgeId());
	if (!currentAge) {
		return;
	}

	text->SetText(Game::getLocalization()->Get("top_age_title") + currentAge->name);
	rows->RemoveAllChildren();

	for (const auto& ageProgress : controller->getNextAgeProgress(players)) {
		const auto* age = controller->getAge(ageProgress.ageId);
		if (!age) {
			continue;
		}

		const auto row = createElement<Urho3D::UIElement>(rows, style, "AgeProgressRow");
		addChildText(row, "AgeProgressText", age->name + " " + Urho3D::String(static_cast<int>(ageProgress.progress * 100.f)) + "%",
		             style);
		const auto bar = createElement<Urho3D::ProgressBar>(row, style, "AgeProgressBar");
		bar->SetRange(1.f);
		bar->SetValue(ageProgress.progress);
		bar->SetVisible(true);
	}
}

void TopInfoPanel::createBody() {
	text = addChildText(window, "AgeTitle", style);
	rows = createElement<Urho3D::UIElement>(window, style, "AgeProgressRows");
}
