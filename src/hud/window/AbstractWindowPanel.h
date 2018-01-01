#pragma once
#include <Urho3D/UI/Window.h>

namespace Urho3D {
	class String;
}

class AbstractWindowPanel :public Urho3D::Object
{
	URHO3D_OBJECT(AbstractWindowPanel, Object)

	AbstractWindowPanel(Urho3D::XMLFile* _style);
	virtual ~AbstractWindowPanel();
	Urho3D::String& getStyleName();
	Urho3D::Window * createWindow();
	virtual void setVisible(bool enable);
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window * window;
	Urho3D::String styleName;
private:
	virtual void createBody()=0;
};

