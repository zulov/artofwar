#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Sprite.h>

class MiniMapPanel : public AbstractWindowPanel
{
public:
	MiniMapPanel(Urho3D::XMLFile* _style);
	~MiniMapPanel();
	void createEmpty(int parts);
	void update();
private:
	void createBody() override;
	Urho3D::Sprite* spr;
	Urho3D::Image* minimap;
	Urho3D::Texture2D* text;
	unsigned* heightMap;
	int indexUpdate = 0;
	int indexPerUpdate;
};
