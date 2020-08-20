#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Sprite.h>

constexpr char MINI_MAP_BUTTON_NUMBER = 5;


class MiniMapPanel : public AbstractWindowPanel {
public:
	void initColors();
	explicit MiniMapPanel(Urho3D::XMLFile* _style);
	~MiniMapPanel();
	void createEmpty(int parts);
	void changeValue(uint32_t* data, bool& changed, unsigned val) const;
	void update();

private:
	void createBody() override;
	void HandleButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMiniMapClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	Urho3D::Sprite* spr;
	Urho3D::Image* minimap;
	Urho3D::Texture2D* text;
	unsigned* heightMap;
	int indexUpdate = 0;
	int indexPerUpdate;
	bool checks[MINI_MAP_BUTTON_NUMBER];
	unsigned *unitsColors,
	         *buildingColors,
	         *resourceColors;
};
