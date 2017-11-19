#include "MiniMapPanel.h"
#include "Game.h"
#include "hud/MySprite.h"
#include <Urho3D/Graphics/Texture2D.h>
#include "simulation/env/Enviroment.h"
#include "simulation/env/ContentInfo.h"


MiniMapPanel::MiniMapPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "MiniMapWindow";
}


MiniMapPanel::~MiniMapPanel() {
	delete heightMap;
}

void MiniMapPanel::createEmpty(int parts) {
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

void MiniMapPanel::update() {
	IntVector2 size = spr->GetSize();
	Enviroment* env = Game::get()->getEnviroment();
	uint32_t* data = (uint32_t*)minimap->GetData();

	float xinc = 1.0f / (size.x_);
	float yinc = 1.0f / (size.y_);

	int partIndex = 0;
	for (; partIndex < indexPerUpdate && indexUpdate < size.y_ * size.x_; ++partIndex, ++indexUpdate) {
		float yVal = 1 - yinc * (indexUpdate / size.x_);
		float xVal = 0 + xinc * (indexUpdate % size.x_);

		content_info* ci = env->getContentInfo(Vector2(xVal, yVal), Vector2(xVal + xinc, yVal - yinc));
		if (ci->hasBuilding()) {
			*(data + indexUpdate) = 0xFF900000;
		} else if (ci->hasResource()) {
			*(data + indexUpdate) = 0xFF001000;
		} else if (ci->hasUnit()) {
			*(data + indexUpdate) = 0xFFCF0000;
		} else {
			*(data + indexUpdate) = heightMap[indexUpdate];
		}
	}
	if (indexUpdate >= size.y_ * size.x_) {
		indexUpdate = 0;
	}

	text->SetData(minimap);

	spr->SetTexture(text);
}

void MiniMapPanel::createBody() {
	spr = window->CreateChild<Sprite>();

	UIElement * row = window->CreateChild<UIElement>();
	row->SetStyle("MiniMapListRow", style);

	buttons = new Button*[MINI_MAP_BUTTON_NUMBER];
	for (int i = 0; i < MINI_MAP_BUTTON_NUMBER; ++i) {
		buttons[i] = row->CreateChild<Button>();
		buttons[i]->SetStyle("MiniMapIcon", style);
	}

	IntVector2 size = spr->GetSize();

	spr->SetMaxSize(size);
	minimap = new Image(Game::get()->getContext());

	minimap->SetSize(size.x_, size.y_, 4);

	text = new Texture2D(Game::get()->getContext());
	text->SetData(minimap);

	spr->SetTexture(text);
	heightMap = new unsigned[size.x_ * size.x_];

	
	
}
