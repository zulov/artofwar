#include "InGameMenuPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>
#include "hud/MySprite.h"
#include "hud/ButtonUtils.h"
#include <Urho3D/Resource/ResourceCache.h>


InGameMenuPanel::InGameMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "InGameMenuWindow";
}

InGameMenuPanel::~InGameMenuPanel() {
}

void InGameMenuPanel::createBody() {
	Urho3D::Texture2D* texture = Game::get()->getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/menu.png");

	MySprite* sprite = createSprite(texture, style, "Sprite");
	toggleButton = simpleButton(sprite, style, "InGameToggledButton");
	Game::get()->getUI()->GetRoot()->AddChild(toggleButton);
	window->SetPriority(1);
	toggleButton->SetPriority(2);
}
