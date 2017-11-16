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
	int a = minimap->GetHeight();

	int idR = 0;
	float div = 5;
	float dy = size.y_;
	float dx = size.x_;

	float xinc = 1.0f / (size.x_ - 1);
	float yinc = 1.0f / (size.y_ - 1);
	float yVal = 1;
	float xVal = 0;
	for (short y = size.y_; y > 0; --y) {
		for (short x = 0; x < size.x_; ++x) {

			float val = env->getGroundHeightPercent(yVal, xVal, div);
			//std::cout << val << " ";
			*(data + idR) = 0xFF003000 + unsigned(val) * 0X100;
			++idR;
			
			xVal += xinc;
		}
		xVal = 0;
		yVal -= yinc;
		//std::cout << std::endl;
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
