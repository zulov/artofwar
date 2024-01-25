#include "MiniMapPanel.h"

#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "camera/CameraManager.h"
#include "database/db_other_struct.h"
#include "hud/UiUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "env/ContentInfo.h"
#include "env/Environment.h"


MiniMapPanel::MiniMapPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style)
	: EventPanel(root, _style, "MiniMapWindow", {GameState::RUNNING, GameState::PAUSE}) {
	std::fill_n(checks, MINI_MAP_BUTTON_NUMBER, true);
}


MiniMapPanel::~MiniMapPanel() {
	delete[] heightMap;
	delete minimap;
	delete[] unitsColors;
	delete[] buildingColors;
	text->Release();
}

void MiniMapPanel::initColors() {
	auto colorsSize = Game::getDatabase()->getPlayerColors().size();

	unitsColors = new unsigned[colorsSize];
	buildingColors = new unsigned[colorsSize];

	std::fill_n(unitsColors, colorsSize, 0xFF505050);
	std::fill_n(buildingColors, colorsSize, 0xFF505050);
	std::fill_n(resourceColors, RESOURCES_SIZE, 0xFF808080);

	for (int i = 0; i < RESOURCES_SIZE; ++i) {
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

	const float xinc = 1.0f / (size.x_ - 1);
	const float yinc = 1.0f / (size.y_ - 1);
	float yVal = 1;
	float xVal = 0;
	for (short y = size.y_; y > 0; --y) {
		for (short x = 0; x < size.x_; ++x) {
			float val = env->getGroundHeightPercentScaled( xVal, yVal, div);
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
	const auto size = spr->GetSize();
	auto env = Game::getEnvironment();
	auto* data = (uint32_t*)minimap->GetData();

	const float xinc = 1.0f / size.x_;
	const float yinc = 1.0f / size.y_;
	const auto activePlayer = Game::getPlayersMan()->getActivePlayerID();
	bool changed = false;
	for (int partIndex = 0; partIndex < indexPerUpdate && indexUpdate < size.y_ * size.x_; ++partIndex, ++indexUpdate) {
		const float yVal = 1 - yinc * (indexUpdate / size.x_);
		const float xVal = 0 + xinc * (indexUpdate % size.x_);

		content_info* ci = env->getContentInfo({xVal + xinc / 2, yVal - yinc / 2}, checks, activePlayer);

		if (checks[2] && ci->hasBuilding) {
			changeValue(data, changed, buildingColors[ci->biggestBuilding()]);
		} else if (checks[1] && ci->hasResource) {
			changeValue(data, changed, resourceColors[ci->biggestResource()]);
		} else if ((checks[3] || checks[4]) && ci->hasUnit) {
			changeValue(data, changed, unitsColors[ci->biggestUnits()]);
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
	const auto row = createElement<Urho3D::UIElement>(window, style, "MiniMapListRow");

	for (int i = 0; i < MINI_MAP_BUTTON_NUMBER; ++i) {
		const auto texture = getTexture("textures/hud/icon/mm/minimap" + Urho3D::String(i) + ".png");

		const auto box = createElement<Urho3D::CheckBox>(row, style, "MiniMapCheckBox");
		createSprite(box, texture, style, "MiniMapSprite");

		box->SetVar("Num", i);

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
	const auto element = (Urho3D::CheckBox*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	const int id = element->GetVar("Num").GetInt();

	checks[id] = element->IsChecked();
}

void MiniMapPanel::HandleMiniMapClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::Sprite*>(eventData[Urho3D::Click::P_ELEMENT].GetVoidPtr());
	const auto begin = element->GetScreenPosition();
	const auto size = element->GetSize();
	float x = eventData[Urho3D::Click::P_X].GetInt() - begin.x_;
	float y = size.y_ - (eventData[Urho3D::Click::P_Y].GetInt() - begin.y_);

	Game::getCameraManager()->changePositionInPercent(x / size.x_, y / size.y_);
}
