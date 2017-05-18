#include "Hud.h"
#include "Game.h"


Hud::Hud() {
	window = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(window);

	someMenuExample();

	XMLFile* style = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");
	SharedPtr<Cursor> cursor(new Cursor(Game::get()->getContext()));
	cursor->SetStyleAuto(style);
	Game::get()->getUI()->SetCursor(cursor);

	cursor->SetPosition(Game::get()->getGraphics()->GetWidth() / 2, Game::get()->getGraphics()->GetHeight() / 2);

}


Hud::~Hud() {
}

template <std::size_t SIZE>
void Hud::populateList(Font* font, DropDownList* dropDownList, std::array<String, SIZE> elements) {
	for (String mode : elements) {
		Text* text = new Text(Game::get()->getContext());
		text->SetText(mode);
		text->SetFont(font, 12);
		dropDownList->AddItem(text);
	}
}

void Hud::initDropDownList(DropDownList* dropDownList) {
	dropDownList->SetFixedHeight(24);
	dropDownList->SetFixedWidth(100);
	dropDownList->SetResizePopup(true);

	XMLFile* xmlFile = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");

	dropDownList->SetStyleAuto(xmlFile);
}

void Hud::someMenuExample() {
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");
	Texture2D* warriorIcon = Game::get()->getCache()->GetResource<Texture2D>("textures/warriorIcon64.png");
	window->SetMinWidth(256);
	window->SetMinHeight(50);
	window->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
	window->SetAlignment(HA_LEFT, VA_BOTTOM);
	window->SetName("Window");
	window->SetTexture(wood);
	window->SetTiled(true);

	Font* font = Game::get()->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	DropDownList* dropDownList = new DropDownList(Game::get()->getContext());
	DropDownList* dropDownList1 = new DropDownList(Game::get()->getContext());

	std::array<String, 3> modes{{"Select","Deploy","Build"}};
	populateList(font, dropDownList, modes);

	std::array<String, 3> modes1{{"Select","Deploy","Build"}};
	populateList(font, dropDownList1, modes1);

	initDropDownList(dropDownList);
	initDropDownList(dropDownList1);

	window->AddChild(dropDownList);
	window->AddChild(dropDownList1);

	Button* button = new Button(Game::get()->getContext());
	XMLFile* xmlFile = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");
	button->SetStyle("Icon", xmlFile);

	button->SetMinHeight(24);
	button->SetTexture(warriorIcon);
	
	button->SetFixedSize(64, 64);
	button->SetTiled(true);
	window->AddChild(button);


}

void Hud::createStaticHud(String msg) {
	Text* instructionText = Game::get()->getUI()->GetRoot()->CreateChild<Text>();
	instructionText->SetText(msg);
	instructionText->SetFont(Game::get()->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	instructionText->SetTextAlignment(HA_CENTER);
	instructionText->SetHorizontalAlignment(HA_LEFT);
	instructionText->SetVerticalAlignment(VA_TOP);
	instructionText->SetPosition(0, 0);
}

void Hud::createLogo() {
	Texture2D* logoTexture = Game::get()->getCache()->GetResource<Texture2D>("textures/minimap.png");
	if (!logoTexture) {
		return;
	}
	Urho3D::Sprite* logoSprite_ = Game::get()->getUI()->GetRoot()->CreateChild<Sprite>();

	logoSprite_->SetTexture(logoTexture);

	int textureWidth = logoTexture->GetWidth();
	int textureHeight = logoTexture->GetHeight();

	logoSprite_->SetScale(256.0f / textureWidth);
	logoSprite_->SetSize(textureWidth, textureHeight);
	logoSprite_->SetHotSpot(textureWidth, textureHeight);
	logoSprite_->SetAlignment(HA_RIGHT, VA_BOTTOM);
	logoSprite_->SetOpacity(0.9f);
}

void Hud::createDebugHud() {
	XMLFile* xmlFile = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");

	DebugHud* debugHud = Game::get()->getEngine()->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
}

void Hud::createConsole() {
	XMLFile* xmlFile = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");

	Console* console = Game::get()->getEngine()->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);
}

void Hud::updateHud(Benchmark* benchmark, CameraManager* cameraManager) {
	Urho3D::String msg = "FPS: " + String(benchmark->getLastFPS());
	msg += "\navg FPS: " + String(benchmark->getAverageFPS());
	msg += "\nLoops: " + String(benchmark->getLoops());
	msg += "\nCamera: ";
	msg += "\n\t" + cameraManager->getInfo();

	Game::get()->getUI()->GetRoot()->RemoveChild(fpsText);

	fpsText = Game::get()->getUI()->GetRoot()->CreateChild<Text>();
	fpsText->SetText(msg);
	fpsText->SetFont(Game::get()->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	fpsText->SetTextAlignment(HA_LEFT);
	fpsText->SetHorizontalAlignment(HA_LEFT);
	fpsText->SetVerticalAlignment(VA_TOP);
	fpsText->SetPosition(0, 20);
}
