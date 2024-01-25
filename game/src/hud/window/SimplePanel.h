#pragma once
#include <magic_enum.hpp>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Graphics/Texture2D.h>
#include "GameState.h"
#include "initializer_list"

namespace Urho3D
{
	class UIElement;
	class Window;
	class XMLFile;
}

class SimplePanel {
public:
	SimplePanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, Urho3D::String styleName,
	            std::initializer_list<GameState> active);
	virtual ~SimplePanel();
	void createWindow();
	void updateStateVisibility(GameState state);

	virtual void setVisible(bool enable);
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window* window;
	Urho3D::UIElement* root;
	Urho3D::String styleName, bodyStyle;

	bool visibleAt[magic_enum::enum_count<GameState>()];
	Urho3D::Texture2D* getTexture(Urho3D::String path) const;
private:
	virtual void createBody() =0;
};
