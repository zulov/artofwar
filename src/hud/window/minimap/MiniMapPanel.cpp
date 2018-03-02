#include "MiniMapPanel.h"
#include "Game.h"
#include "hud/MySprite.h"
#include <Urho3D/Graphics/Texture2D.h>
#include "simulation/env/Enviroment.h"
#include "simulation/env/ContentInfo.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <LinearMath/btVector3.h>
#include "database/DatabaseCache.h"
#include <Urho3D/UI/CheckBox.h>
#include "player/PlayersManager.h"
#include <Urho3D/UI/UIEvents.h>
#include "GameState.h"


MiniMapPanel::MiniMapPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "MiniMapWindow";

	for (int i = 0; i < PLAYER_COLORS_NUMBER_DB; ++i) {
		unitsColors[i] = 0xFF505050;
		buildingColors[i] = 0xFF505050;
	}

	int size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* res = Game::get()->getDatabaseCache()->getResource(i);
		if (res) {
			resourceColors[i] = res->mini_map_color;
		} else {
			resourceColors[i] = 0xFF808080;
		}
	}

	std::fill_n(checks, MINI_MAP_BUTTON_NUMBER, true);

	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


MiniMapPanel::~MiniMapPanel() {
	delete[] heightMap;
	delete minimap;
	text->Release();
	clear_vector(hudElements);
}

void MiniMapPanel::initColors() {
	for (int i = 0; i < PLAYER_COLORS_NUMBER_DB; ++i) {
		unitsColors[i] = 0xFF505050;
		buildingColors[i] = 0xFF505050;
	}

	PlayersManager* playerManager = Game::get()->getPlayersManager();
	for (auto player : playerManager->getAllPlayers()) {
		db_player_colors* col = Game::get()->getDatabaseCache()->getPlayerColor(player->getColor());

		if (col) {
			unitsColors[player->getId()] = col->unit;
			buildingColors[player->getId()] = col->building;
		}
	}
}

void MiniMapPanel::createEmpty(int parts) {
	initColors();
	IntVector2 size = spr->GetSize();
	Enviroment* env = Game::get()->getEnviroment();
	uint32_t* data = (uint32_t*)minimap->GetData();

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

void MiniMapPanel::changeValue(uint32_t* data, bool& changed, unsigned val) {
	if (*(data + indexUpdate) != val) {
		changed = true;
		*(data + indexUpdate) = val;
	}
}

void MiniMapPanel::update() {
	IntVector2 size = spr->GetSize();
	Enviroment* env = Game::get()->getEnviroment();
	uint32_t* data = (uint32_t*)minimap->GetData();

	float xinc = 1.0f / (size.x_);
	float yinc = 1.0f / (size.y_);

	int partIndex = 0;
	bool changed = false;
	for (; partIndex < indexPerUpdate && indexUpdate < size.y_ * size.x_; ++partIndex, ++indexUpdate) {
		const float yVal = 1 - yinc * (indexUpdate / size.x_);
		const float xVal = 0 + xinc * (indexUpdate % size.x_);
		int activePlayer = Game::get()->getPlayersManager()->getActivePlayer()->getId();

		content_info* ci = env->getContentInfo(Vector2(xVal, yVal), Vector2(xVal + xinc, yVal - yinc), checks, activePlayer);

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
			changeValue(data, changed, 0xFF222222);
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

Urho3D::Sprite* MiniMapPanel::getSpriteToSubscribe() {
	return spr;
}

void MiniMapPanel::changeMiniMapType(short id, bool val) {
	checks[id] = val;
}

void MiniMapPanel::createBody() {
	spr = window->CreateChild<Sprite>();
	spr->SetEnabled(true);
	UIElement* row = window->CreateChild<UIElement>();
	row->SetStyle("MiniMapListRow", style);

	checksElements.reserve(MINI_MAP_BUTTON_NUMBER);
	hudElements.reserve(MINI_MAP_BUTTON_NUMBER);

	for (int i = 0; i < MINI_MAP_BUTTON_NUMBER; ++i) {
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/mm/minimap" + String(i) + ".png");

		MySprite* sprite = createSprite(texture, style, "MiniMapSprite");
		CheckBox* box = row->CreateChild<CheckBox>();
		box->SetStyle("MiniMapCheckBox", style);
		checksElements.push_back(box);
		box->AddChild(sprite);
		HudData* hudElement = new HudData(checksElements.at(i));
		hudElement->setId(i, ObjectType::ENTITY);
		hudElements.push_back(hudElement);
		checksElements.at(i)->SetVar("HudElement", hudElement);

		SubscribeToEvent(box, E_CLICK, URHO3D_HANDLER(MiniMapPanel, HandleButton));
	}

	IntVector2 size = spr->GetSize();

	spr->SetMaxSize(size);
	minimap = new Image(Game::get()->getContext());

	minimap->SetSize(size.x_, size.y_, 4);

	text = new Texture2D(Game::get()->getContext());
	text->SetData(minimap);

	spr->SetTexture(text);
	heightMap = new unsigned[size.x_ * size.y_];
}

void MiniMapPanel::HandleButton(StringHash eventType, VariantMap& eventData) {
	CheckBox* element = (CheckBox*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudData* hudElement = (HudData *)element->GetVar("HudElement").GetVoidPtr();

	changeMiniMapType(hudElement->getId(), element->IsChecked());
}
