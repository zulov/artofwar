#pragma once

enum class ObjectType : char;

namespace Urho3D
{
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
	void reset();

	Urho3D::ProgressBar* getNext(ObjectType type);
	void commit();

private:
	Urho3D::ProgressBar** createSet(Urho3D::String style,  int size) const;

	Urho3D::ProgressBar** resourceBars;
	Urho3D::ProgressBar** playerBars; 
	Urho3D::UIElement* root; 

	int idx = 0;
};
