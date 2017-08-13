#pragma once
#include <Urho3D/UI/Window.h>

namespace Urho3D {
	class String;
}

class AbstractWindowPanel
{
public:
	AbstractWindowPanel(Urho3D::XMLFile* _style);
	~AbstractWindowPanel();
	Urho3D::Window * createWindow(const Urho3D::String& styleName);
	void setVisible(bool enable);
protected:
	Urho3D::XMLFile* style;
	Urho3D::Window * window;
private:
	virtual void createBody()=0;
};

