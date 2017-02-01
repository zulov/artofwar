#pragma once
#include <Urho3D/Input/Input.h>
using namespace Urho3D;
class Control {
public:
	Control();
	~Control();
	void handleKeyUp(StringHash /*eventType*/, VariantMap& eventData);
	void handleKeyDown(StringHash /*eventType*/, VariantMap& eventData);
	void handleMouseModeRequest(StringHash /*eventType*/, VariantMap& eventData);
	void handleMouseModeChange(StringHash /*eventType*/, VariantMap& eventData);
	void input(float timeStep);

};

