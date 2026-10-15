#include "DebugPanel.h"
#include "colors/ColorPaletteRepo.h"
#include "hud/UiUtils.h"

#include <algorithm>
#include <array>
#include <cstdio>

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>

DebugPanel::DebugPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "MyDebugHudWindow",
                                                              {GameState::RUNNING, GameState::PAUSE}) {
}


void DebugPanel::setText(float avgLow, float avgMiddle,
                         float avgHighest, const float* tickAvgs, const Urho3D::String& camInfo) {
	summaryText->SetText(Urho3D::String("FPS low|middle|high: ") + Urho3D::String(avgLow) + "|" +
			Urho3D::String(avgMiddle) + "|" + Urho3D::String(avgHighest) + "\nFPS by tick slot:");
	cameraText->SetText(Urho3D::String("Camera: \n\t") + camInfo);

	std::array<unsigned, FRAMES_IN_PERIOD> rankedTicks{};
	unsigned rankedCount = 0;
	for (unsigned i = 0; i < FRAMES_IN_PERIOD; ++i) {
		if (tickAvgs[i] > 0.0f) {
			rankedTicks[rankedCount++] = i;
		}
	}
	std::ranges::sort(rankedTicks.begin(), rankedTicks.begin() + rankedCount, [tickAvgs](unsigned lhs, unsigned rhs) {
		return tickAvgs[lhs] != tickAvgs[rhs] ? tickAvgs[lhs] < tickAvgs[rhs] : lhs < rhs;
	});

	for (auto* tickText : tickTexts) {
		tickText->SetColor(ColorPaletteRepo::getFpsNeutralColor());
	}
	const unsigned lowRankCount = std::min(3u, (rankedCount + 1) / 2);
	const unsigned highRankCount = std::min(3u, rankedCount / 2);
	for (unsigned rank = 0; rank < lowRankCount; ++rank) {
		tickTexts[rankedTicks[rank]]->SetColor(ColorPaletteRepo::getFpsRankColor(rank));
	}
	for (unsigned rank = 0; rank < highRankCount; ++rank) {
		tickTexts[rankedTicks[rankedCount - highRankCount + rank]]->SetColor(
			ColorPaletteRepo::getFpsBestRankColor(3 - highRankCount + rank));
	}

	for (int i = 0; i < FRAMES_IN_PERIOD; ++i) {
		char buf[8];
		if (tickAvgs[i] > 0.0f) {
			std::snprintf(buf, sizeof(buf), "%5.0f", tickAvgs[i]);
		} else {
			std::snprintf(buf, sizeof(buf), "%5s", "-");
		}
		tickTexts[i]->SetText(buf);
	}
}

void DebugPanel::createBody() {
	summaryText = addChildText(window, "MyText", style);
	// TODO: Consider extending Urho3D Text with per-range colors so the FPS slots can share one label. The current
	// SetColor() API applies to the whole element; per-range colors would need to carry character color spans into
	// Text's glyph batch generation. That would avoid one Text element per slot while preserving ranked colors.
	Urho3D::UIElement* row{};
	for (unsigned i = 0; i < FRAMES_IN_PERIOD; ++i) {
		if (i % 10 == 0) {
			row = window->CreateChild<Urho3D::UIElement>();
			row->SetLayout(Urho3D::LM_HORIZONTAL);
		}
		tickTexts[i] = addChildText(row, "MyText", style);
		tickTexts[i]->SetFixedSize(42, 18);
	}
	cameraText = addChildText(window, "MyText", style);
}
