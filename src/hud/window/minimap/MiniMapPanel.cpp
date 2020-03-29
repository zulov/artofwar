#include "MiniMapPanel.h"
#include "GameState.h"
#include "camera/CameraManager.h"
#include "database/DatabaseCache.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/Environment.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>


MiniMapPanel::MiniMapPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "MiniMapWindow",
                                                                          {GameState::RUNNING, GameState::PAUSE}) {
	std::fill_n(checks, MINI_MAP_BUTTON_NUMBER, true);
}


MiniMapPanel::~MiniMapPanel() {
	delete[] heightMap;
	delete minimap;
	delete[] unitsColors;
	delete[] buildingColors;
	delete[] resourceColors;
	text->Release();
}

void MiniMapPanel::initColors() {
	auto colorsSize = Game::getDatabase()->getPlayerColors().size();
	auto resourceSize = Game::getDatabase()->getResourceSize();
	unitsColors = new unsigned[colorsSize];
	buildingColors = new unsigned[colorsSize];
	resourceColors = new unsigned[resourceSize];

	std::fill_n(unitsColors, colorsSize, 0xFF505050);
	std::fill_n(buildingColors, colorsSize, 0xFF505050);
	std::fill_n(resourceColors, resourceSize, 0xFF808080);

	for (int i = 0; i < resourceSize; ++i) {
		const auto res = Game::getDatabase()->getResource(i);
		if (res) {
			resourceColors[i] = res->mini_map_color;
		}
	}

	PlayersManager* playerManager = Game::getPlayersMan();
	for (auto player : playerManager->getAllPlayers()) {
		db_player_colors* col = Game::getDatabase()->getPlayerColor(player->getColor());

		if (col) {
			unitsColors[player->getId()] = col->unit;
			buildingColors[player->getId()] = col->building;
		}
	}
}

void MiniMapPanel::createEmpty(int parts) {
	initColors();
	auto size = spr->GetSize();
	const auto env = Game::getEnvironment();
	const auto data = (uint32_t*)minimap->GetData();

	int idR = 0;
	float div = 5;

	float xinc = 1.0f / (size.x_ - 1);
	float yinc = 1.0f / (size.y_ - 1);
	float yVal = 1;
	float xVal = 0;
	for (short y = size.y_; y > 0; --y) {
		for (short x = 0; x < size.x_; ++x) {
			float val = env->getGroundHeightPercent(yVal, xVal, div);
			*(data + idR) = 0xFF003000 + unsigned(val) * 0X100;
			heightMap[idR] = *(data + idR);
			++idR;
			xVal += xinc;
		}
		xVal = 0;
		yVal -= yinc;
	}

	text->SetData(minimap);

	spr->SetTexture(text);
	indexPerUpdate = size.x_ * size.y_ / parts + 1;
}

void MiniMapPanel::changeValue(uint32_t* data, bool& changed, unsigned val) const {
	if (*(data + indexUpdate) != val) {
		changed = true;
		*(data + indexUpdate) = val;
	}
}

void MiniMapPanel::update() {
	auto size = spr->GetSize();
	auto env = Game::getEnvironment();
	auto* data = (uint32_t*)minimap->GetData();

	float xinc = 1.0f / size.x_;
	float yinc = 1.0f / size.y_;

	bool changed = false;
	for (int partIndex = 0; partIndex < indexPerUpdate && indexUpdate < size.y_ * size.x_; ++partIndex, ++indexUpdate) {
		const float yVal = 1 - yinc * (indexUpdate / size.x_);
		const float xVal = 0 + xinc * (indexUpdate % size.x_);
		int activePlayer = Game::getPlayersMan()->getActivePlayerID();

		content_info* ci = env->getContentInfo({xVal + xinc / 2, yVal - yinc / 2}, checks, activePlayer);

		if (checks[2] && ci->hasBuilding) {
			unsigned char player = ci->biggestBuilding();
			changeValue(data, changed, buildingColors[player]);
		} else if (checks[1] && ci->hasResource) {
			unsigned char resID = ci->biggestResource();
			changeValue(data, changed, resourceColors[resID]);
		} else if ((checks[3] || checks[4]) && ci->hasUnit) {
			unsigned char player = ci->biggestUnits();
			changeValue(data, changed, unitsColors[player]);
		} else if (checks[0]) {
			changeValue(data, changed, heightMap[indexUpdate]);
		} else {
			changeValue(data, changed, 0x00FFFFFF);
		}
	}
	if (indexUpdate >= size.y_ * size.x_) {
		indexUpdate = 0;
	}
	if (changed) {
		text->SetData(minimap);
		spr->SetTexture(text);
	}
}

void MiniMapPanel::createBody() {
	spr = window->CreateChild<Urho3D::Sprite>();
	spr->SetEnabled(true);
	auto row = createElement<Urho3D::UIElement>(window, style, "MiniMapListRow");

	checksElements.reserve(MINI_MAP_BUTTON_NUMBER);

	for (int i = 0; i < MINI_MAP_BUTTON_NUMBER; ++i) {
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
		>("textures/hud/icon/mm/minimap" + Urho3D::String(i) + ".png");

		const auto box = createElement<Urho3D::CheckBox>(row, style, "MiniMapCheckBox");
		createSprite(box, texture, style, "MiniMapSprite");
		checksElements.push_back(box);

		checksElements.at(i)->SetVar("Num", i);

		SubscribeToEvent(box, Urho3D::E_CLICK, URHO3D_HANDLER(MiniMapPanel, HandleButton));
	}

	auto size = spr->GetSize();

	spr->SetMaxSize(size);
	minimap = new Urho3D::Image(Game::getContext());

	minimap->SetSize(size.x_, size.y_, 4);

	text = new Urho3D::Texture2D(Game::getContext());
	text->SetData(minimap);

	spr->SetTexture(text);
	heightMap = new unsigned[size.x_ * size.y_];
	SubscribeToEvent(spr, Urho3D::E_CLICK, URHO3D_HANDLER(MiniMapPanel, HandleMiniMapClick));
}

void MiniMapPanel::HandleButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = (Urho3D::CheckBox*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	int id = element->GetVar("Num").GetInt();

	checks[id] = element->IsChecked();
}

void MiniMapPanel::HandleMiniMapClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::Sprite*>(eventData[Urho3D::Click::P_ELEMENT].GetVoidPtr());
	auto begin = element->GetScreenPosition();
	auto size = element->GetSize();
	float x = eventData[Urho3D::Click::P_X].GetInt() - begin.x_;
	float y = size.y_ - (eventData[Urho3D::Click::P_Y].GetInt() - begin.y_);

	Game::getCameraManager()->changePosition(x / size.x_, y / size.y_);
}
