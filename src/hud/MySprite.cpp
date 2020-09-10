#include "MySprite.h"
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Texture2D.h>

const char* blendModeNames[] =
{
	"replace",
	"add",
	"multiply",
	"alpha",
	"addalpha",
	"premulalpha",
	"invdestalpha",
	"subtract",
	"subtractalpha",
	0
};
const char* horizontalAlignments[] =
{
	"Left",
	"Center",
	"Right",
	"Custom",
	0
};

const char* verticalAlignments[] =
{
	"Top",
	"Center",
	"Bottom",
	"Custom",
	0
};

MySprite::MySprite(Urho3D::Context* context) :
	Sprite(context) {
}

void MySprite::RegisterObject(Urho3D::Context* context) {
	context->RegisterFactory<MySprite>();
	using namespace Urho3D;
	URHO3D_ATTRIBUTE("Percent HotSpot", Vector2, percentHotspot, Vector2(0.5, 0.5), Urho3D::AM_DEFAULT);
	URHO3D_ATTRIBUTE("My Size", IntVector2, mySize, IntVector2::ZERO, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Name", GetName, SetName, String, String::EMPTY, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Position", GetPosition, SetPosition, Vector2, Vector2::ZERO, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Size", GetSize, SetSize, IntVector2, IntVector2::ZERO, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Hotspot", GetHotSpot, SetHotSpot, IntVector2, IntVector2::ZERO, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Scale", GetScale, SetScale, Vector2, Vector2::ONE, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Rotation", GetRotation, SetRotation, float, 0.0f, AM_FILE);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Texture", GetTextureAttr, SetTextureAttr, ResourceRef, ResourceRef(Texture2D::GetTypeStatic()),
		AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Image Rect", GetImageRect, SetImageRect, IntRect, IntRect::ZERO, AM_FILE);
	URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Blend Mode", GetBlendMode, SetBlendMode, BlendMode, blendModeNames, 0, AM_FILE);
	URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Horiz Alignment", GetHorizontalAlignment, SetHorizontalAlignment, HorizontalAlignment,
		horizontalAlignments, HA_LEFT, AM_FILE);
	URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Vert Alignment", GetVerticalAlignment, SetVerticalAlignment, VerticalAlignment, verticalAlignments,
		VA_TOP, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Priority", GetPriority, SetPriority, int, 0, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Opacity", GetOpacity, SetOpacity, float, 1.0f, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Color", GetColorAttr, SetColor, Color, Color::WHITE, AM_FILE);
	//	URHO3D_ATTRIBUTE("Top Left Color", Color, color_[0], Color::WHITE, AM_FILE);
	//	URHO3D_ATTRIBUTE("Top Right Color", Color, color_[1], Color::WHITE, AM_FILE);
	//	URHO3D_ATTRIBUTE("Bottom Left Color", Color, color_[2], Color::WHITE, AM_FILE);
	//	URHO3D_ATTRIBUTE("Bottom Right Color", Color, color_[3], Color::WHITE, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Is Visible", IsVisible, SetVisible, bool, true, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Use Derived Opacity", GetUseDerivedOpacity, SetUseDerivedOpacity, bool, true, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Max Size", GetMaxSize, SetMaxSize, IntVector2, IntVector2::ZERO, AM_FILE);
	//URHO3D_ATTRIBUTE("Variables", VariantMap, vars_, Variant::emptyVariantMap, AM_FILE);
}
