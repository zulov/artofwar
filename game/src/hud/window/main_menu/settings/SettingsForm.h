#pragma once
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UIElement.h>

struct SettingsForm {
	SettingsForm() = default;
	SettingsForm(const SettingsForm&) = delete;
	int resolution;
	int maxFps;
	int minFps;
	int textureQuality;
	int hudSize;
	bool fullScreen;
	bool vSync;
	bool shadow;

	static SettingsForm* getFromElement(Urho3D::VariantMap& eventData) {
		auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
		return static_cast<SettingsForm *>(element->GetVar("SettingsForm").GetVoidPtr());
	}
};
