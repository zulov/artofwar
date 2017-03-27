#include "Hud.h"



Hud::Hud(Context* context, UI* _ui, ResourceCache* _cache, Graphics* _graphics) {
	ui = _ui;
	cache = _cache;
	window = new Window(context);
	ui->GetRoot()->AddChild(window);

	//someMenuExample(context);

	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	SharedPtr<Cursor> cursor(new Cursor(context));
	cursor->SetStyleAuto(style);
	ui->SetCursor(cursor);

	cursor->SetPosition(_graphics->GetWidth() / 2, _graphics->GetHeight() / 2);
}


Hud::~Hud() {}

void Hud::someMenuExample(Context* context) {
	window->SetMinWidth(384);
	window->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
	window->SetAlignment(HA_RIGHT, VA_TOP);
	window->SetName("Window");

	CheckBox* checkBox = new CheckBox(context);
	checkBox->SetName("CheckBox");

	Button* button = new Button(context);
	button->SetName("Button");
	button->SetMinHeight(24);

	LineEdit* lineEdit = new LineEdit(context);
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
	Text* instructionText = ui->GetRoot()->CreateChild<Text>();
	instructionText->SetText(msg);
	instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	instructionText->SetTextAlignment(HA_CENTER);
	instructionText->SetHorizontalAlignment(HA_LEFT);
	instructionText->SetVerticalAlignment(VA_TOP);
	instructionText->SetPosition(0, 0);
}

void Hud::createLogo() {
	Texture2D* logoTexture = cache->GetResource<Texture2D>("textures/minimap.png");
	if (!logoTexture) {
		return;
	}
	Urho3D::Sprite *logoSprite_ = ui->GetRoot()->CreateChild<Sprite>();

	logoSprite_->SetTexture(logoTexture);

	int textureWidth = logoTexture->GetWidth();
	int textureHeight = logoTexture->GetHeight();

	logoSprite_->SetScale(256.0f / textureWidth);
	logoSprite_->SetSize(textureWidth, textureHeight);
	logoSprite_->SetHotSpot(textureWidth, textureHeight);
	logoSprite_->SetAlignment(HA_RIGHT, VA_BOTTOM);
	logoSprite_->SetOpacity(0.9f);
}

void Hud::createDebugHud(SharedPtr<Engine> engine) {
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	DebugHud* debugHud = engine->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
}

void Hud::createConsole(SharedPtr<Engine> engine) {
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	Console* console = engine->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);
}

void Hud::updateHud(Benchmark * benchmark, CameraManager *cameraManager) {
	Urho3D::String msg = "FPS: " + String(benchmark->getLastFPS());
	msg += "\navg FPS: " + String(benchmark->getAverageFPS());
	msg += "\nLoops: " + String(benchmark->getLoops());
	msg += "\nCamera: ";
	msg += "\n\t" + cameraManager->getInfo();

	ui->GetRoot()->RemoveChild(fpsText);

	fpsText = ui->GetRoot()->CreateChild<Text>();
	fpsText->SetText(msg);
	fpsText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	fpsText->SetTextAlignment(HA_LEFT);
	fpsText->SetHorizontalAlignment(HA_LEFT);
	fpsText->SetVerticalAlignment(VA_TOP);
	fpsText->SetPosition(0, 20);
}
