#include "Hud.h"
#include "Game.h"


Hud::Hud() {
	buttons = new std::vector<HudElement*>();
	lists = new std::vector<HudElement*>();
	db_graph_settings* graphSettings = Game::get()->getDatabaseCache()->getGraphSettings(0);
	style = Game::get()->getCache()->GetResource<XMLFile>("UI/" + graphSettings->style);
	hudSize = Game::get()->getDatabaseCache()->getHudSize(graphSettings->hud_size);//TODO z settings to wziac
	font = Game::get()->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf");

	createMenu();
	createBuild();
	createTop();
	createUnits();
	createSelectedInfo();

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
	dropDownList->SetFixedWidth(120);
	dropDownList->SetResizePopup(true);

	dropDownList->SetStyleAuto(style);
	menuWindow->AddChild(dropDownList);
}

void Hud::createMenu() {
	menuWindow = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(menuWindow);
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");

	menuWindow->SetFixedWidth(2 * (hudSize->icon_size_x + hudSize->space_size_x));
	menuWindow->SetFixedHeight(2 * (hudSize->icon_size_y + hudSize->space_size_y));
	menuWindow->SetLayout(LM_VERTICAL, hudSize->space_size_x, IntRect(hudSize->space_size_x, hudSize->space_size_y, hudSize->space_size_x, hudSize->space_size_y));
	menuWindow->SetAlignment(HA_LEFT, VA_BOTTOM);
	menuWindow->SetName("Window");
	menuWindow->SetTexture(wood);
	menuWindow->SetTiled(true);

	DropDownList* dropDownList = new DropDownList(Game::get()->getContext());

	std::array<String, 3> modes{{"Select","Build","Deploy"}};
	populateList(font, dropDownList, modes);

	initDropDownList(dropDownList);

	HudElement* hudElement = new HudElement(dropDownList);
	dropDownList->SetVar("HudElement", hudElement);
	lists->push_back(hudElement);
}

void Hud::createBuild() {
	buildWindow = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(buildWindow);
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");
	buildWindow->SetMinWidth(512);
	buildWindow->SetMinHeight(64);
	buildWindow->SetLayout(LM_HORIZONTAL, hudSize->space_size_x, IntRect(hudSize->space_size_x, hudSize->space_size_y, hudSize->space_size_x, hudSize->space_size_y));
	buildWindow->SetAlignment(HA_LEFT, VA_BOTTOM);
	buildWindow->SetPosition(2 * (hudSize->icon_size_x + hudSize->space_size_x), 0);
	buildWindow->SetTexture(wood);
	buildWindow->SetTiled(true);
	buildWindow->SetVisible(false);

	std::array<BuildingType, 4> buildings{{HOUSE,TOWER,BARRACKS,ARCHERY_RANGE}};
	createBuildingIcons(buildings);
}

void Hud::createUnits() {
	unitsWindow = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(unitsWindow);
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");
	unitsWindow->SetMinWidth(512);
	unitsWindow->SetMinHeight(64);
	unitsWindow->SetLayout(LM_HORIZONTAL, hudSize->space_size_x, IntRect(hudSize->space_size_x, hudSize->space_size_y, hudSize->space_size_x, hudSize->space_size_y));
	unitsWindow->SetAlignment(HA_LEFT, VA_BOTTOM);
	unitsWindow->SetPosition(2 * (hudSize->icon_size_x + hudSize->space_size_x), 0);
	unitsWindow->SetTexture(wood);
	unitsWindow->SetTiled(true);
	unitsWindow->SetVisible(false);

	std::array<UnitType, 5> units{{WARRIOR, PIKEMAN,CAVALRY,ARCHER ,WORKER}};
	createUnitIcons(units);
}

template <std::size_t SIZE>
void Hud::createBuildingIcons(std::array<BuildingType, SIZE> buildings) {
	for (BuildingType type : buildings) {
		db_building_type* buidling = Game::get()->getDatabaseCache()->getBuildingType(type);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/" + hudSize->name + "/" + buidling->icon);
		Button* button = new Button(Game::get()->getContext());

		button->SetStyle("Icon", style);
		button->SetTexture(texture);
		button->SetFixedSize(hudSize->icon_size_x, hudSize->icon_size_y);
		button->SetTiled(true);

		HudElement* hudElement = new HudElement(button);
		hudElement->setBuildingType(type);
		button->SetVar("HudElement", hudElement);
		buttons->push_back(hudElement);
		buildWindow->AddChild(button);
	}
}

template <std::size_t SIZE>
void Hud::createUnitIcons(std::array<UnitType, SIZE> units) {
	for (UnitType type : units) {
		db_unit_type* unit = Game::get()->getDatabaseCache()->getUnitType(type);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/" + hudSize->name + "/" + unit->icon);
		Button* button = new Button(Game::get()->getContext());

		button->SetStyle("Icon", style);
		button->SetTexture(texture);
		button->SetFixedSize(hudSize->icon_size_x, hudSize->icon_size_y);
		button->SetTiled(true);

		HudElement* hudElement = new HudElement(button);
		hudElement->setUnitType(type);
		button->SetVar("HudElement", hudElement);
		buttons->push_back(hudElement);
		unitsWindow->AddChild(button);
	}
}

void Hud::createTop() {
	topWindow = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(topWindow);
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");

	topWindow->SetMinWidth(512);
	topWindow->SetMinHeight(24);
	topWindow->SetLayout(LM_HORIZONTAL, 6, IntRect(6, 6, 6, 6));
	topWindow->SetAlignment(HA_RIGHT, VA_TOP);
	topWindow->SetName("Window");
	topWindow->SetTexture(wood);
	topWindow->SetTiled(true);

	String modes[] = {"Z³oto","Kamieñ","Drewno"};
	for (String mode : modes) {
		Text* text = new Text(Game::get()->getContext());
		text->SetText(mode);
		text->SetFont(font, 12);
		topWindow->AddChild(text);
	}

}

void Hud::createSelectedInfo() {
	selectedInfoWindow = new Window(Game::get()->getContext());
	Game::get()->getUI()->GetRoot()->AddChild(selectedInfoWindow);
	Texture2D* wood = Game::get()->getCache()->GetResource<Texture2D>("textures/wood.png");

	selectedInfoWindow->SetMinWidth(512);
	selectedInfoWindow->SetMinHeight(64);
	selectedInfoWindow->SetLayout(LM_HORIZONTAL, 6, IntRect(6, 6, 6, 6));
	selectedInfoWindow->SetAlignment(HA_CENTER, VA_BOTTOM);
	selectedInfoWindow->SetName("Window");
	selectedInfoWindow->SetTexture(wood);
	selectedInfoWindow->SetTiled(true);

}

void Hud::createStaticHud(String msg) {
	Text* instructionText = Game::get()->getUI()->GetRoot()->CreateChild<Text>();
	instructionText->SetText(msg);
	instructionText->SetFont(font, 12);
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
	DebugHud* debugHud = Game::get()->getEngine()->CreateDebugHud();
	debugHud->SetDefaultStyle(style);
}

void Hud::createConsole() {
	Console* console = Game::get()->getEngine()->CreateConsole();
	console->SetDefaultStyle(style);
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
	fpsText->SetFont(font, 12);
	fpsText->SetTextAlignment(HA_LEFT);
	fpsText->SetHorizontalAlignment(HA_LEFT);
	fpsText->SetVerticalAlignment(VA_TOP);
	fpsText->SetPosition(0, 20);
}

std::vector<HudElement*>* Hud::getButtonsToSubscribe() {
	return buttons;
}

std::vector<HudElement*>* Hud::getListsToSubscribe() {
	return lists;
}

void Hud::updateState(ControlsState state) {
	switch (state) {
	case SELECT:
		buildWindow->SetVisible(false);
		unitsWindow->SetVisible(false);
		break;
	case BUILD:
		buildWindow->SetVisible(true);
		unitsWindow->SetVisible(false);
		break;
	case DEPLOY:
		buildWindow->SetVisible(false);
		unitsWindow->SetVisible(true);
		break;
	default: ;
	}
}

void Hud::updateSelected(SelectedInfo* selectedInfo) {
	if (selectedInfo->hasChanged()) {
		selectedInfoWindow->RemoveAllChildren();
		Text* text = new Text(Game::get()->getContext());
		text->SetText((*selectedInfo->getMessage()));
		text->SetFont(font, 12);
		selectedInfoWindow->AddChild(text);
		String** lines = selectedInfo->getLines();
		for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
			if ((lines[i]) != nullptr) {
				Text* text = new Text(Game::get()->getContext());
				text->SetText((*lines[i]));
				text->SetFont(font, 12);

				Button* button = new Button(Game::get()->getContext());
				button->SetStyleAuto(style);
				
				button->AddChild(text);
				button->SetFixedSize(hudSize->icon_size_x, hudSize->icon_size_y);
				selectedInfoWindow->AddChild(button);
			}
		}
	}
}
