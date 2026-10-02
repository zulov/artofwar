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

void TopInfoPanel::update(const WorldAgeController* controller, const std::vector<Player*>& players,
							  unsigned totalTicks) const {
	if (!controller) {
		return;
	}

	const auto* currentAge = controller->getAge(controller->getCurrentAgeId());
	if (!currentAge) {
		return;
	}

	const auto localization = Game::getLocalization();
	text->SetText(localization->Get("top_age_title") + localization->Get(currentAge->name));
	rows->RemoveAllChildren();
	const auto ageProgress = controller->getNextAgeProgress(players);
	if (ageProgress.empty()) {
		timeoutText->SetVisible(false);
		timeoutBar->SetVisible(false);
		return;
	}

	for (const auto& progress : ageProgress) {
		const auto* age = controller->getAge(progress.ageId);
		if (!age) {
			continue;
		}

		const auto row = createElement<Urho3D::UIElement>(rows, style, "AgeProgressRow");
		addChildText(row, "AgeProgressText", localization->Get(age->name) + " "
																 + Urho3D::String(static_cast<int>(progress.progress * 100.f)) + "%",
					 style);
		const auto bar = createElement<Urho3D::ProgressBar>(row, style, "AgeProgressBar");
		bar->SetRange(1.f);
		bar->SetValue(progress.progress);
		bar->SetVisible(true);
	}

	const auto timeoutProgress = controller->getTimeoutProgress(totalTicks);
	timeoutText->SetText(localization->Get("top_age_timeout") + " "
						 + Urho3D::String(static_cast<int>(timeoutProgress * 100.f)) + "%");
	timeoutText->SetVisible(true);
	timeoutBar->SetRange(1.f);
	timeoutBar->SetValue(timeoutProgress);
	timeoutBar->SetVisible(true);
}

void TopInfoPanel::createBody() {
	text = addChildText(window, "AgeTitle", style);
	rows = createElement<Urho3D::UIElement>(window, style, "AgeProgressRows");
	timeoutText = addChildText(window, "AgeTimeoutText", style);
	timeoutBar = createElement<Urho3D::ProgressBar>(window, style, "AgeTimeoutBar");
}
