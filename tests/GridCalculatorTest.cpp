#include "pch.h"
#include "../game/src/env/GridCalculator.h"
#include "../game/include/Urho3D/Math/Vector3.h"

class GridCalculatorFixture : public ::testing::Test {
public:
	static GridCalculator* gc;
	static GridCalculator* gcBig;

protected:
	static void SetUpTestCase() {
		// Kod uruchamiany przed pierwszym testem wewnatrz `Test Case`
		gc = new GridCalculator(4, 8);
		gcBig = new GridCalculator(8, 8);
	}

	static void TearDownTestCase() {
		delete gc;
		delete gcBig;
		// Kod uruchamiany po ostatnim tescie wewnatrz `Test Case`
	}
};

GridCalculator* GridCalculatorFixture::gc = nullptr;
GridCalculator* GridCalculatorFixture::gcBig = nullptr;


TEST_F(GridCalculatorFixture, Center) {
	EXPECT_EQ(gc->getCenter(0), Urho3D::Vector2(-3.f, -3.f));
	EXPECT_EQ(gc->getCenter(6), Urho3D::Vector2(-1.f, 1.f));
	EXPECT_EQ(gc->getCenter(9), Urho3D::Vector2(1.f, -1.f));
	EXPECT_EQ(gc->getCenter(15), Urho3D::Vector2(3.f, 3.f));
}

TEST_F(GridCalculatorFixture, GetIndex) {
	EXPECT_EQ(gc->getIndex(-3.f), 0);
	EXPECT_EQ(gc->getIndex(3.f), 3);
	EXPECT_EQ(gc->getIndex(-0.1f), 1);
	EXPECT_EQ(gc->getIndex(0.1f), 2);
	EXPECT_EQ(gc->getIndex(-5.f), 0);
	EXPECT_EQ(gc->getIndex(5.f), 3);

	EXPECT_EQ(gc->getCords(0), Urho3D::IntVector2(0, 0));
	EXPECT_EQ(gc->getCords(3), Urho3D::IntVector2(0, 3));
	EXPECT_EQ(gc->getCords(6), Urho3D::IntVector2(1, 2));
	EXPECT_EQ(gc->getCords(9), Urho3D::IntVector2(2, 1));
	EXPECT_EQ(gc->getCords(15), Urho3D::IntVector2(3, 3));
}


TEST_F(GridCalculatorFixture, CombineTest) {
	for (auto i = -3.f; i < 3.f; i += 2.f) {
		for (auto j = -3.f; j < 3.f; j += 2.f) {
			auto index = gc->indexFromPosition(i, j);
			auto center = gc->getCenter(index);

			EXPECT_TRUE(abs(center.x_ - i) <= 0.f);
			EXPECT_TRUE(abs(center.y_ - j) <= 0.f);
		}
	}
}

