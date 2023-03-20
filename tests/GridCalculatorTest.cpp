#include "pch.h"
#include "../game/src/env/GridCalculator.h"
#include "../game/include/Urho3D/Math/Vector3.h"

TEST(GridCalculator, GridCalculator1) {
	auto* gc = new GridCalculator(512, 1024);
	auto a = gc->getCenter(65012);
	std::cout << a.x_;
	//delete gc;
	EXPECT_EQ(a.x_, -259);
}
