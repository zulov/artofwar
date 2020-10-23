#pragma once
#include <magic_enum.hpp>

#include "GameState.h"
#include "initializer_list"
#include "Urho3D/Container/Str.h"
#include "Urho3D/Core/Object.h"

namespace Urho3D
{
	class Window;
	class XMLFile;
}

class AbstractWindowPanel : public Urho3D::Object {
URHO3D_OBJECT(AbstractWindowPanel, Object)

	explicit AbstractWindowPanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
	                             std::initializer_list<GameState> active);
	virtual ~AbstractWindowPanel();
	void createWindow();
	void updateStateVisibilty(GameState state);

	Urho3D::String& getStyleName() { return styleName; }
	virtual void setVisible(bool enable);
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window* window;
	Urho3D::String styleName, bodyStyle;

	bool visibleAt[magic_enum::enum_count<GameState>()];
private:
	virtual void createBody() =0;
};
