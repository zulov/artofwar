#pragma once

enum class ObjectType : char;

namespace Urho3D {
	class UIElement;
	class ProgressBar;
	class String;
}

class HealthBarProvider {
public:
	HealthBarProvider() = default;
	HealthBarProvider(const HealthBarProvider& rhs) = delete;
	~HealthBarProvider();
	void init();
	void reset(int size, ObjectType type);
	void hide(Urho3D::ProgressBar* bar);
	Urho3D::ProgressBar* createNew(const Urho3D::String& style);

	Urho3D::ProgressBar* getNext(ObjectType type);

private:
	Urho3D::ProgressBar** createSet(int size) const;

	Urho3D::ProgressBar** resourceBars{};
	Urho3D::ProgressBar** playerBars{};
	Urho3D::UIElement* root;

	int playerIdx = 0;
	int resIdx = 0;
};
