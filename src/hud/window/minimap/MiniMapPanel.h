#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Button.h>
#include <vector>
#include "MiniMapType.h"

#define MINI_MAP_BUTTON_NUMBER 4


class MiniMapPanel : public AbstractWindowPanel
{
public:
	MiniMapPanel(Urho3D::XMLFile* _style);
	~MiniMapPanel();
	void createEmpty(int parts);
	void changeValue(uint32_t* data, bool& changed, unsigned val);
	void update();
	std::vector<Urho3D::Button*>* getButtonsMiniMapToSubscribe();
	void changeMiniMapType(short id);
private:
	void createBody() override;
	Urho3D::Sprite* spr;
	Urho3D::Image* minimap;
	Urho3D::Texture2D* text;
	MiniMapType type;
	std::vector<Urho3D::Button*> *buttons;
	unsigned* heightMap;
	int indexUpdate = 0;
	int indexPerUpdate;
};
