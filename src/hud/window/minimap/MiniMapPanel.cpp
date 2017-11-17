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
}

void MiniMapPanel::createEmpty() {
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
			++idR;
			xVal += xinc;
		}
		xVal = 0;
		yVal -= yinc;
	}

	text->SetData(minimap);

	spr->SetTexture(text);
}

void MiniMapPanel::update() {
	IntVector2 size = spr->GetSize();
	Enviroment* env = Game::get()->getEnviroment();
	uint32_t* data = (uint32_t*)minimap->GetData();

	int idR = 0;

	float xinc = 1.0f / (size.x_);
	float yinc = 1.0f / (size.y_);
	float yVal = 1;
	float xVal = 0;

	for (short y = size.y_; y > 0; --y) {
		for (short x = 0; x < size.x_; ++x) {

			content_info *ci = env->getContentInfo(Vector2(xVal, yVal), Vector2(xVal + xinc, yVal - yinc));
			if (ci->hasUnit()) {
				*(data + idR) = 0xFF0000AA;
			}else {
				*(data + idR) = 0xFF003000;
			}
			
			++idR;
			xVal += xinc;
		}
		xVal = 0;
		yVal -= yinc;
	}

	text->SetData(minimap);

	spr->SetTexture(text);
}

void MiniMapPanel::createBody() {
	spr = window->CreateChild<Sprite>();

	IntVector2 size = spr->GetSize();
	if (size.x_ < size.y_) {
		size.y_ = size.x_;
	} else {
		size.x_ = size.y_;
	}
	spr->SetMaxSize(size);
	minimap = new Image(Game::get()->getContext());

	minimap->SetSize(size.x_, size.y_, 4);

	text = new Texture2D(Game::get()->getContext());
	text->SetData(minimap);

	spr->SetTexture(text);
}
