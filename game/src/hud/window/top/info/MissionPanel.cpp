#include "MissionPanel.h"

#include <Urho3D/Math/Color.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/ScrollView.h>

#include "Game.h"
#include "database/DatabaseCache.h"
#include "database/db_struct.h"
#include "hud/UiUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/MissionController.h"
#include "utils/StringUtils.h"

namespace {
Urho3D::String timeText(unsigned ticks) {
	const auto seconds = ticks / FRAMES_IN_PERIOD;
	return to2DigString(static_cast<unsigned char>((seconds / 60) % 100)) + ":" +
		to2DigString(static_cast<unsigned char>(seconds % 60));
}

Urho3D::String goalProgress(const MissionGoalState& state, unsigned totalTicks) {
	const auto& goal = *state.goal;
	switch (goal.type) {
	case MissionGoalType::SURVIVE_UNTIL:
	case MissionGoalType::SURVIVE_WITH_BUILDING_UNTIL:
	case MissionGoalType::SCORE_LEAD_AT_TIME:
		if (goal.type == MissionGoalType::SCORE_LEAD_AT_TIME && state.deadlineEvaluated) {
			return l10nFormat("mission_progress_count", static_cast<int>(state.deadlineScore),
				static_cast<int>(state.deadlineEnemyScore));
		}
		return timeText(state.complete ? 0 : goal.timeTicks > totalTicks ? goal.timeTicks - totalTicks : 0);
	case MissionGoalType::ELIMINATE_PLAYER:
		return state.complete ? Urho3D::String::EMPTY : l10nFormat("mission_progress_count", 0, 1);
	case MissionGoalType::ELIMINATE_TEAM:
	case MissionGoalType::ELIMINATE_ALL:
	case MissionGoalType::ELIMINATE_UNITS:
	case MissionGoalType::ELIMINATE_BUILDINGS:
		return l10nFormat("mission_goal_remaining", static_cast<int>(state.value));
	case MissionGoalType::KILL_UNITS:
	case MissionGoalType::RESOURCE_STOCK:
	case MissionGoalType::RESOURCE_GATHERED:
	case MissionGoalType::BUILDING_COUNT:
	case MissionGoalType::SCORE_REACH:
		return l10nFormat("mission_progress_count", static_cast<int>(state.value), static_cast<int>(state.target));
	}
	return {};
}
}

MissionPanel::MissionPanel(Urho3D::UIElement* root, Urho3D::XMLFile* style)
	: SimplePanel(root, style, "MissionGoalsWindow", {GameState::RUNNING, GameState::PAUSE}) {}

void MissionPanel::update() {
	const auto* controller = Game::getMissionController();
	auto* player = Game::getPlayersMan()->getActivePlayer();
	if (controller->getMap()->goals.empty()) {
		window->SetVisible(false);
		return;
	}
	window->SetVisible(true);
	const auto totalTicks = Game::getFrameInfo()->getTotalTicks();
	size_t rowIndex = 0;
	for (const auto& state : controller->getGoals()) {
		const auto& goal = *state.goal;
		Urho3D::String line;
		const auto progress = goalProgress(state, totalTicks);
		switch (goal.type) {
		case MissionGoalType::RESOURCE_STOCK:
		case MissionGoalType::RESOURCE_GATHERED: {
			static const char* resourceKeys[] = {"resource_food", "resource_wood", "resource_stone", "resource_gold"};
			const auto resource = Game::getLocalization()->Get(resourceKeys[goal.targetId]);
			line = l10nFormat(goal.type == MissionGoalType::RESOURCE_STOCK ? "mission_resource_stock" :
				"mission_resource_gathered", static_cast<int>(state.target), resource.CString());
			break;
		}
		case MissionGoalType::BUILDING_COUNT:
			if (goal.targetId >= 0) {
				const auto* building = Game::getDatabase()->getBuilding(goal.targetId);
				const auto name = Game::getLocalization()->Get(building->name);
				line = l10nFormat("mission_building_count", static_cast<int>(state.target), name.CString());
			} else {
				line = l10nFormat("mission_building_count_any", static_cast<int>(state.target));
			}
			break;
		case MissionGoalType::SCORE_REACH:
			line = l10nFormat("mission_score_reach", static_cast<int>(state.target));
			break;
		case MissionGoalType::KILL_UNITS:
			line = l10nFormat("mission_kill_units", static_cast<int>(state.target));
			break;
		case MissionGoalType::ELIMINATE_PLAYER:
			line = l10nFormat("mission_eliminate_player",
				Game::getPlayersMan()->getPlayer(static_cast<unsigned char>(goal.targetPlayer))->getName().CString());
			break;
		case MissionGoalType::SCORE_LEAD_AT_TIME: {
			const auto playerScore = static_cast<int>(state.deadlineEvaluated ? state.deadlineScore : player->getScore());
			auto* enemy = goal.targetPlayer >= 0
				? Game::getPlayersMan()->getPlayer(static_cast<unsigned char>(goal.targetPlayer))
				: Game::getPlayersMan()->getEnemyFor(player->getId());
			const auto enemyScore = static_cast<int>(state.deadlineEvaluated ? state.deadlineEnemyScore :
				enemy->getScore());
			line = l10nFormat("mission_score_lead", playerScore, enemyScore);
			break;
		}
		case MissionGoalType::ELIMINATE_TEAM:
			line = l10nFormat("mission_eliminate_specific_team", static_cast<int>(
				Game::getPlayersMan()->getPlayer(static_cast<unsigned char>(goal.targetPlayer))->getTeam()));
			break;
		default:
			line = Game::getLocalization()->Get(goal.textKey);
			break;
		}
		if (!progress.Empty()) line.Append("  ").Append(progress);
		auto* row = getRow(rowIndex++);
		row->SetText(line);
		row->SetColor(state.complete ? Urho3D::Color(0.5f, 0.5f, 0.5f) : Urho3D::Color::WHITE);
	}
	if (controller->getResult() != MissionResult::RUNNING) {
		auto* result = getRow(rowIndex++);
		result->SetText(Game::getLocalization()->Get(controller->getResult() == MissionResult::VICTORY
			? "mission_victory" : "mission_defeat"));
		result->SetColor(controller->getResult() == MissionResult::VICTORY ? Urho3D::Color::GREEN : Urho3D::Color::RED);
	}
	for (; rowIndex < rowTexts.size(); ++rowIndex) rowTexts[rowIndex]->SetVisible(false);
}

Urho3D::Text* MissionPanel::getRow(size_t index) {
	if (index == rowTexts.size()) rowTexts.push_back(addChildText(rows, "MissionGoalText", style));
	auto* row = rowTexts[index];
	row->SetVisible(true);
	return row;
}

void MissionPanel::createBody() {
	title = addChildText(window, "MissionGoalsTitle", style);
	title->SetText(Game::getLocalization()->Get("mission_goals_title"));
	auto* scrollView = createElement<Urho3D::ScrollView>(window, style, "MissionGoalsScrollView");
	rows = createElement<Urho3D::UIElement>(scrollView, style, "MissionGoalsRows");
	scrollView->SetContentElement(rows);
}
