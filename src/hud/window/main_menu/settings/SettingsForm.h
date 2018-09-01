#pragma once
#include <Urho3D/UI/UIEvents.h>

struct SettingsForm
{
	int resolution;
	bool fullScreen;
	int maxFps;
	int minFps;
	bool vSync;
	int textureQuality;
	bool shadow;
	int hudSize;

	static SettingsForm* getFromElement(Urho3D::VariantMap& eventData) {
		auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
		return static_cast<SettingsForm *>(element->GetVar("SettingsForm").GetVoidPtr());
	}
};
