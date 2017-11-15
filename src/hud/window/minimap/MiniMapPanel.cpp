#include "MiniMapPanel.h"
#include "Game.h"
#include "hud/MySprite.h"
#include <Urho3D/Graphics/Texture2D.h>
#include "simulation/env/Enviroment.h"


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
	float dy = size.y_;
	float dx = size.x_;
	for (short y = 0; y < dy; ++y) {
		for (short x = 0; x < dx; ++x) {
			float val = env->getGroundHeightPercent((dy - 1 - y) / dy, x / dx, div);

			*(data + idR) = 0xFF003000 + unsigned(val) * 0X100;
			++idR;
		}
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
