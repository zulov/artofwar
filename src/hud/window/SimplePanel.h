#pragma once
#include <magic_enum.hpp>
#include <Urho3D/Container/Str.h>
#include "GameState.h"
#include "initializer_list"

namespace Urho3D
{
	class Window;
	class XMLFile;
}

class SimplePanel {
public:
	SimplePanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
	            std::initializer_list<GameState> active);
	virtual ~SimplePanel();
	void createWindow();
	void updateStateVisibility(GameState state);

	virtual void setVisible(bool enable);
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window* window;
	Urho3D::String styleName, bodyStyle;

	bool visibleAt[magic_enum::enum_count<GameState>()];
private:
	virtual void createBody() =0;
};
