#pragma once
#include <vector>

namespace Urho3D {
	class Texture2D;
	class String;
	class Text;
	class XMLFile;
	class UIElement;
	class ProgressBar;
	class Button;
}

constexpr char MAX_SELECTED_IN_BUTTON = 5;

class MySprite;
class Physical;

class SelectedHudElement {
public:
	explicit SelectedHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style);
	~SelectedHudElement() = default;

	Urho3D::Button* getButton() const;
	void hide() const;
	void show() const;

	void setText(const Urho3D::String& msg) const;
	void hideText() const;
	void setTexture(Urho3D::Texture2D* texture) const;
	void add(std::vector<Physical*>& physicals, int start, int end);
	std::vector<Physical*>& getSelected();
private:
	Urho3D::Button* button;
	Urho3D::ProgressBar* bar;
	Urho3D::Text* text;
	MySprite* icon;

	std::vector<Physical*> selected;
};
