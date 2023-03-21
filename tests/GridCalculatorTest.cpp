#include "pch.h"
#include "../game/src/env/GridCalculator.h"
#include "../game/include/Urho3D/Math/Vector3.h"

class GridCalculatorFixture : public ::testing::Test {
public:
    static GridCalculator* gc;
protected:

    static void SetUpTestCase() {
        // Kod uruchamiany przed pierwszym testem wewn¹trz `Test Case`
        gc = new GridCalculator(4, 8);
    }

    static void TearDownTestCase() {
        delete gc;
        // Kod uruchamiany po ostatnim teœcie wewn¹trz `Test Case`
    }
};
GridCalculator* GridCalculatorFixture::gc = nullptr;


TEST_F(GridCalculatorFixture, Center1) {
	EXPECT_EQ(gc->getCenter(0), Urho3D::Vector2(-3.f, -3.f));
	EXPECT_EQ(gc->getCenter(6), Urho3D::Vector2(-1.f, 1.f));
	EXPECT_EQ(gc->getCenter(9), Urho3D::Vector2(1.f, -1.f));
	EXPECT_EQ(gc->getCenter(15), Urho3D::Vector2(3.f, 3.f) );
}

TEST_F(GridCalculatorFixture, GetIndex) {


    EXPECT_EQ(gc->getIndex(-3.f), 0);
    EXPECT_EQ(gc->getIndex(3.f), 3);
    EXPECT_EQ(gc->getIndex(-0.1f), 1);
    EXPECT_EQ(gc->getIndex(0.1f), 2);
    EXPECT_EQ(gc->getIndex(-5.f), 0);
    EXPECT_EQ(gc->getIndex(5.f), 3);
}