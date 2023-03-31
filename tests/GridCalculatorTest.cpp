#include "pch.h"
#include "../game/src/env/GridCalculator.h"
#include "../game/include/Urho3D/Math/Vector3.h"

class GridCalculatorFixture : public ::testing::Test {
public:
	static GridCalculator* gc;
	static GridCalculator* gcBig;

protected:
	static void SetUpTestCase() {
		// Kod uruchamiany przed pierwszym testem wewn¹trz `Test Case`
		gc = new GridCalculator(4, 8);
		gcBig = new GridCalculator(8, 8);
	}

	static void TearDownTestCase() {
		delete gc;
		delete gcBig;
		// Kod uruchamiany po ostatnim teœcie wewn¹trz `Test Case`
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

	EXPECT_EQ(gc->getIndexes(0), Urho3D::IntVector2(0, 0));
	EXPECT_EQ(gc->getIndexes(3), Urho3D::IntVector2(0, 3));
	EXPECT_EQ(gc->getIndexes(6), Urho3D::IntVector2(1, 2));
	EXPECT_EQ(gc->getIndexes(9), Urho3D::IntVector2(2, 1));
	EXPECT_EQ(gc->getIndexes(15), Urho3D::IntVector2(3, 3));
}


TEST_F(GridCalculatorFixture, GetBiggestDiff) {
	EXPECT_EQ(gc->getBiggestDiff(0, { 0, 1, 2, 6, 7 }), 3);
	EXPECT_EQ(gc->getBiggestDiff(15, { 1, 6, 7 }), 3);
}


TEST_F(GridCalculatorFixture, CombineTest) {
	for (auto i = -3.f; i < 3.f; i += 2.f) {
		for (auto j = -3.f; j < 3.f; j += 2.f) {
			auto index = gc->indexFromPosition(i, j);
			auto center = gc->getCenter(index);

			EXPECT_TRUE(abs(center.x_ - i)<=0.f);
			EXPECT_TRUE(abs(center.y_ - j) <= 0.f);
		}
	}
}

TEST_F(GridCalculatorFixture, GetShiftCords) {
	EXPECT_EQ(gcBig->getShiftCords(-18), Urho3D::IntVector2(-2, -2));
	EXPECT_EQ(gcBig->getShiftCords(-17), Urho3D::IntVector2(-2, -1));
	EXPECT_EQ(gcBig->getShiftCords(-16), Urho3D::IntVector2(-2, 0));
	EXPECT_EQ(gcBig->getShiftCords(-15), Urho3D::IntVector2(-2, 1));
	EXPECT_EQ(gcBig->getShiftCords(-14), Urho3D::IntVector2(-2, 2));

	EXPECT_EQ(gcBig->getShiftCords(-10), Urho3D::IntVector2(-1, -2));
	EXPECT_EQ(gcBig->getShiftCords(-9), Urho3D::IntVector2(-1, -1));
	EXPECT_EQ(gcBig->getShiftCords(-8), Urho3D::IntVector2(-1, 0));
	EXPECT_EQ(gcBig->getShiftCords(-7), Urho3D::IntVector2(-1, 1));
	EXPECT_EQ(gcBig->getShiftCords(-6), Urho3D::IntVector2(-1, 2));

	EXPECT_EQ(gcBig->getShiftCords(-2), Urho3D::IntVector2(0, -2));
	EXPECT_EQ(gcBig->getShiftCords(-1), Urho3D::IntVector2(0, -1));
	EXPECT_EQ(gcBig->getShiftCords(0),  Urho3D::IntVector2(0, 0));
	EXPECT_EQ(gcBig->getShiftCords(1),  Urho3D::IntVector2(0, 1));
	EXPECT_EQ(gcBig->getShiftCords(2),  Urho3D::IntVector2(0, 2));

	EXPECT_EQ(gcBig->getShiftCords(6), Urho3D::IntVector2(1, -2));
	EXPECT_EQ(gcBig->getShiftCords(7), Urho3D::IntVector2(1, -1));
	EXPECT_EQ(gcBig->getShiftCords(8), Urho3D::IntVector2(1, 0));
	EXPECT_EQ(gcBig->getShiftCords(9), Urho3D::IntVector2(1, 1));
	EXPECT_EQ(gcBig->getShiftCords(10), Urho3D::IntVector2(1, 2));
	 
	EXPECT_EQ(gcBig->getShiftCords(14), Urho3D::IntVector2(2, -2));
	EXPECT_EQ(gcBig->getShiftCords(15), Urho3D::IntVector2(2, -1));
	EXPECT_EQ(gcBig->getShiftCords(16), Urho3D::IntVector2(2, 0));
	EXPECT_EQ(gcBig->getShiftCords(17), Urho3D::IntVector2(2, 1));
	EXPECT_EQ(gcBig->getShiftCords(18), Urho3D::IntVector2(2, 2));

	EXPECT_EQ(gcBig->getShiftCords(21), Urho3D::IntVector2(3, -3));
	EXPECT_EQ(gcBig->getShiftCords(-21), Urho3D::IntVector2(-3, 3));
	EXPECT_EQ(gcBig->getShiftCords(27), Urho3D::IntVector2(3, 3));
	EXPECT_EQ(gcBig->getShiftCords(-27), Urho3D::IntVector2(-3, -3));


}
