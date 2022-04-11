#include "HealthBarProvider.h"
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/UI.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"

constexpr int PLAYER_SIZE = 5000;
constexpr int RESOURCE_SIZE = 1000;

HealthBarProvider::~HealthBarProvider() {
	delete[]playerBars;
	delete[]resourceBars;
	//TODO bug a kiedy sie pozbywam UI?
}

void HealthBarProvider::init() {
	root = Game::getUI()->GetRoot()->CreateChild<Urho3D::UIElement>();
	resourceBars = createSet(RESOURCE_SIZE);
	playerBars = createSet(PLAYER_SIZE);
}

Urho3D::ProgressBar** HealthBarProvider::createSet(int size) const {
	auto data = new Urho3D::ProgressBar*[size];
	std::fill_n(data, size, nullptr);

	return data;
}

void HealthBarProvider::reset() {
	for (int i = 0; i < Urho3D::Min(playerIdx, PLAYER_SIZE); ++i) {
		hide(playerBars[i]);
	}
	for (int i = 0; i < Urho3D::Min(resIdx, RESOURCE_SIZE); ++i) {
		hide(resourceBars[i]);
	}
	resIdx = 0;
	playerIdx = 0;
}

void HealthBarProvider::hide(Urho3D::ProgressBar* bar) {
	bar->SetEnabled(false);
	bar->SetVisible(false);
}

Urho3D::ProgressBar* HealthBarProvider::createNew(const Urho3D::String& style) {
	Urho3D::ProgressBar* nowy = createElement<Urho3D::ProgressBar>(root, Game::getUI()->GetRoot()->GetDefaultStyle(),
	                                                               style);
	hide(nowy);
	return nowy;
}

Urho3D::ProgressBar* HealthBarProvider::getNext(ObjectType type) {
	Urho3D::ProgressBar* toReturn = nullptr;
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		if (playerIdx < PLAYER_SIZE) {
			if (!playerBars[playerIdx]) {
				playerBars[playerIdx] = createNew("HealthBar");
			}
			toReturn = playerBars[playerIdx];
		}
		++playerIdx;
		break;
	case ObjectType::RESOURCE:
		if (resIdx < RESOURCE_SIZE) {
			if (!resourceBars[resIdx]) {
				resourceBars[resIdx] = createNew("ResHealthBar");
			}
			toReturn = resourceBars[resIdx];
		}
		++resIdx;
	}
	return toReturn;
}
