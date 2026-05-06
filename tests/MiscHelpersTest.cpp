#include "pch.h"

#include "camera/CameraEnums.h"
#include "control/MouseButton.h"
#include "env/path/DrawGridUtils.h"

class MiscHelpersFixture : public ::testing::Test {};

TEST_F(MiscHelpersFixture, MouseHeldConvertsTo2DAndComputesDelta) {
	MouseHeld held;
	held.first = Urho3D::Vector3(1.f, 10.f, 2.f);
	held.second = Urho3D::Vector3(4.f, 20.f, 6.f);

	auto first2d = held.firstAs2D();
	auto second2d = held.secondAs2D();
	auto delta = held.first2Second();

	EXPECT_EQ(first2d, Urho3D::Vector2(1.f, 2.f));
	EXPECT_EQ(second2d, Urho3D::Vector2(4.f, 6.f));
	EXPECT_EQ(delta, Urho3D::Vector2(3.f, 4.f));
	EXPECT_FLOAT_EQ(held.sq2DDist(), 25.f);
}

TEST_F(MiscHelpersFixture, MouseHeldMinMaxUsesXAndZAxes) {
	MouseHeld held;
	held.first = Urho3D::Vector3(5.f, 0.f, -3.f);
	held.second = Urho3D::Vector3(1.f, 0.f, 7.f);

	EXPECT_EQ(held.minMaxX(), std::pair(1.f, 5.f));
	EXPECT_EQ(held.minMaxZ(), std::pair(-3.f, 7.f));
}

TEST_F(MiscHelpersFixture, DrawGridHelpersConvertBetweenIndexAndCoordinates) {
	EXPECT_EQ(getIndex(2, 3, 8), 19);
	EXPECT_EQ(getCords(19, 8), Urho3D::IntVector2(2, 3));
}

TEST_F(MiscHelpersFixture, CameraBehaviorTypeValuesAreStable) {
	EXPECT_EQ(static_cast<int>(CameraBehaviorType::FREE), 0);
	EXPECT_EQ(static_cast<int>(CameraBehaviorType::RTS), 1);
	EXPECT_EQ(static_cast<int>(CameraBehaviorType::TOP), 2);
}
