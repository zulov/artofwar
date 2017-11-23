#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Button.h>
#include <vector>
#include "MiniMapType.h"
#include "defines.h"
#include "database/db_strcut.h"

#define MINI_MAP_BUTTON_NUMBER 5


class MiniMapPanel : public AbstractWindowPanel
{
public:
	MiniMapPanel(Urho3D::XMLFile* _style);
	~MiniMapPanel();
	void createEmpty(int parts);
	void changeValue(uint32_t* data, bool& changed, unsigned val);
	void update();
	std::vector<Urho3D::UIElement*>* getButtonsMiniMapToSubscribe();
	Urho3D::Sprite* getSpriteToSubscribe();
	void changeMiniMapType(short id, bool val);
private:
	void createBody() override;
	Urho3D::Sprite* spr;
	Urho3D::Image* minimap;
	Urho3D::Texture2D* text;
	std::vector<Urho3D::UIElement*>* elements;
	unsigned* heightMap;
	int indexUpdate = 0;
	int indexPerUpdate;
	bool checks[MINI_MAP_BUTTON_NUMBER];
	unsigned unitsColors[PLAYER_COLORS_NUMBER_DB];
	unsigned buildingColors[PLAYER_COLORS_NUMBER_DB];
	unsigned resourceColors[RESOURCE_NUMBER_DB];
};
