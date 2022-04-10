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
	resourceBars = createSet("ResHealthBar", RESOURCE_SIZE);
	playerBars = createSet("HealthBar", PLAYER_SIZE);
}

Urho3D::ProgressBar**
HealthBarProvider::createSet(Urho3D::String style, int size) const {
	auto data = new Urho3D::ProgressBar*[size];

	for (int i = 0; i < size; ++i) {
		data[i] = createElement<Urho3D::ProgressBar>(root, Game::getUI()->GetRoot()->GetDefaultStyle(), style);
		data[i]->SetEnabled(false);
		data[i]->SetVisible(false);
	}
	return data;
}

void HealthBarProvider::reset() {
	for (int i = 0; i < Urho3D::Min(idx, PLAYER_SIZE); ++i) {
		playerBars[i]->SetEnabled(false);
		playerBars[i]->SetVisible(false);
	}
	for (int i = 0; i < Urho3D::Min(idx, RESOURCE_SIZE); ++i) {
		resourceBars[i]->SetEnabled(false);
		resourceBars[i]->SetVisible(false);
	}
	idx = 0;
}

Urho3D::ProgressBar* HealthBarProvider::getNext(ObjectType type) {
	Urho3D::ProgressBar* toReturn = nullptr;
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		if (idx < PLAYER_SIZE) {
			toReturn = playerBars[idx];
		}
		break;
	case ObjectType::RESOURCE:
		if (idx < RESOURCE_SIZE) {
			toReturn = resourceBars[idx];
		}
	}
	++idx;
	return toReturn;
}
