#include "TopPanel.h"
#include "ButtonUtils.h"


TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
}


TopPanel::~TopPanel() {
}

void TopPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getResourceSize();
	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + resource->icon);

		MySprite* sprite = createSprite(texture, style, "SpriteLeft");
		Button* button = simpleButton(sprite, style, "TopButtons");

		window->AddChild(button);
	}
}
