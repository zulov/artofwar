#include "Hud.h"
#include "Game.h"


Hud::Hud() {
	window = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(window);

	//someMenuExample(context);

	XMLFile* style = Game::get()->getCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");
	SharedPtr<Cursor> cursor(new Cursor(Game::get()->getContext()));
	cursor->SetStyleAuto(style);
	Game::get()->getUI()->SetCursor(cursor);

	cursor->SetPosition(Game::get()->getGraphics()->GetWidth() / 2, Game::get()->getGraphics()->GetHeight() / 2);
}


Hud::~Hud() {}

void Hud::someMenuExample() {
	window->SetMinWidth(384);
	window->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
	window->SetAlignment(HA_RIGHT, VA_TOP);
	window->SetName("Window");

	CheckBox* checkBox = new CheckBox(Game::get()->getContext());
	checkBox->SetName("CheckBox");

	Button* button = new Button(Game::get()->getContext());
	button->SetName("Button");
	button->SetMinHeight(24);

	LineEdit* lineEdit = new LineEdit(Game::get()->getContext());
	lineEdit->SetName("LineEdit");
	lineEdit->SetMinHeight(24);

	window->AddChild(checkBox);
	window->AddChild(button);
	window->AddChild(lineEdit);

	checkBox->SetStyleAuto();
	button->SetStyleAuto();
	lineEdit->SetStyleAuto();
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
	Urho3D::Sprite *logoSprite_ = Game::get()->getUI()->GetRoot()->CreateChild<Sprite>();

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

void Hud::updateHud(Benchmark * benchmark, CameraManager *cameraManager) {
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
