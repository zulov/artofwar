#pragma once
#include <Urho3D/UI/Window.h>
#include <unordered_set>


enum class GameState : char;

namespace Urho3D {
	class String;
}

class AbstractWindowPanel : public Urho3D::Object
{
URHO3D_OBJECT(AbstractWindowPanel, Object)

	AbstractWindowPanel(Urho3D::XMLFile* _style);
	virtual ~AbstractWindowPanel();
	Urho3D::Window* createWindow();
	void updateStateVisibilty(GameState state);

	Urho3D::String& getStyleName() { return styleName; }
	virtual void setVisible(bool enable) { window->SetVisible(enable); }
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window* window;
	Urho3D::String styleName;
	Urho3D::String bodyStyle;

	std::unordered_set<GameState> visibleAt;
private:
	virtual void createBody() =0;
};
