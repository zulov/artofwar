#pragma once
#include "GameState.h"
#include <Urho3D/UI/Window.h>

class AbstractWindowPanel : public Urho3D::Object
{
URHO3D_OBJECT(AbstractWindowPanel, Object)

	explicit AbstractWindowPanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
	                             std::initializer_list<GameState> active);
	virtual ~AbstractWindowPanel();
	void createWindow();
	void updateStateVisibilty(GameState state);

	Urho3D::String& getStyleName() { return styleName; }
	virtual void setVisible(bool enable) { window->SetVisible(enable); }
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window* window;
	Urho3D::String styleName;
	Urho3D::String bodyStyle;

	bool visibleAt[GAME_STATE_SIZE];
private:
	virtual void createBody() =0;
};
