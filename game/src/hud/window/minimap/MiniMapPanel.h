#pragma once
#include "hud/window/EventPanel.h"
#include "utils/defines.h"

namespace Urho3D {
	class Texture2D;
	class Image;
	class Sprite;
}

constexpr char MINI_MAP_BUTTON_NUMBER = 5;


class MiniMapPanel : public EventPanel {
public:
	void initColors();
	explicit MiniMapPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
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
	         *buildingColors;
	unsigned *resourceColors;
};
