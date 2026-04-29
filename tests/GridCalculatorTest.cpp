#include "pch.h"
#include "../game/src/env/GridCalculator.h"
#include "../game/src/env/GridCalculatorProvider.h"
#include "../game/src/env/GridCalculatorProvider.cpp"
#include "../game/include/Urho3D/Math/Vector3.h"

class GridCalculatorFixture : public ::testing::Test {
public:
	static GridCalculator* gc;

protected:
	static void SetUpTestCase() {
		gc = new GridCalculator(4, 8);
	}

	static void TearDownTestCase() {
		delete gc;
	}
};

GridCalculator* GridCalculatorFixture::gc = nullptr;


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

			EXPECT_FLOAT_EQ(center.x_, i);
			EXPECT_FLOAT_EQ(center.y_, j);
		}
	}
}

TEST_F(GridCalculatorFixture, ProviderCachesByResolution) {
	auto* first = GridCalculatorProvider::get(16, 64.f);
	auto* second = GridCalculatorProvider::get(16, 128.f);

	EXPECT_EQ(first, second);
	EXPECT_EQ(first->getResolution(), 16);
	EXPECT_FLOAT_EQ(first->getFieldSize(), 4.f);
}

TEST_F(GridCalculatorFixture, ProviderSeparatesDifferentResolutions) {
	auto* first = GridCalculatorProvider::get(32, 64.f);
	auto* second = GridCalculatorProvider::get(64, 64.f);

	EXPECT_NE(first, second);
	EXPECT_EQ(first->getResolution(), 32);
	EXPECT_EQ(second->getResolution(), 64);
}

TEST_F(GridCalculatorFixture, ValidHelpersClampValuesToGridBounds) {
	EXPECT_EQ(gc->getValidLow(-2), 0);
	EXPECT_EQ(gc->getValidLow(2), 2);
	EXPECT_EQ(gc->getValidHigh(2), 2);
	EXPECT_EQ(gc->getValidHigh(5), 3);
	EXPECT_EQ(gc->getValid(-2), 0);
	EXPECT_EQ(gc->getValid(9), 3);
}

TEST_F(GridCalculatorFixture, IsValidIndexChecksCoordinatesAndFlatIndexes) {
	EXPECT_TRUE(gc->isValidIndex(0, 0));
	EXPECT_TRUE(gc->isValidIndex(Urho3D::IntVector2(3, 3)));
	EXPECT_FALSE(gc->isValidIndex(-1, 0));
	EXPECT_FALSE(gc->isValidIndex(0, 4));
	EXPECT_FALSE(gc->isValidIndex(Urho3D::IntVector2(4, 0)));
	EXPECT_TRUE(gc->isValidIndex(15));
	EXPECT_FALSE(gc->isValidIndex(16));
}

TEST_F(GridCalculatorFixture, GetIndexRangeAndDistancesUseFieldSize) {
	const auto indexRange = gc->getIndex(std::pair(-3.f, 0.1f));
	EXPECT_EQ(indexRange.first, 0);
	EXPECT_EQ(indexRange.second, 2);
	EXPECT_FLOAT_EQ(gc->getSqDistance(Urho3D::IntVector2(0, 0), Urho3D::IntVector2(1, 2)), 20.f);
	EXPECT_FLOAT_EQ(gc->getSqDistance(Urho3D::IntVector2(0, 0), 15), 72.f);
	EXPECT_FLOAT_EQ(gc->getFieldSize(), 2.f);
	EXPECT_FLOAT_EQ(gc->getSize(), 8.f);
}

TEST_F(GridCalculatorFixture, BiggestManhattanReturnsFarthestTargetDistance) {
	std::vector<Urho3D::IntVector2> endCords = {
		Urho3D::IntVector2(1, 1),
		Urho3D::IntVector2(3, 2),
		Urho3D::IntVector2(0, 2)
	};

	EXPECT_EQ(gc->getBiggestManhattan(0, endCords), 5);
}

TEST_F(GridCalculatorFixture, IndexFromVectorOverloadsMatchScalarVersion) {
	Urho3D::Vector2 pos2(-3.f, 3.f);
	Urho3D::Vector3 pos3(-3.f, 0.f, 3.f);

	EXPECT_EQ(gc->indexFromPosition(pos2), gc->indexFromPosition(-3.f, 3.f));
	EXPECT_EQ(gc->indexFromPosition(pos3), gc->indexFromPosition(-3.f, 3.f));
}

TEST_F(GridCalculatorFixture, GetCenterOverloadMatchesIndexCenter) {
	EXPECT_EQ(gc->getCenter(2, 1), gc->getCenter(9));
}

TEST_F(GridCalculatorFixture, GetNotSafeIndexCloseSupportsSignedOffsets) {
	EXPECT_EQ(gc->getNotSafeIndexClose(1, 1), 5);
	EXPECT_EQ(gc->getNotSafeIndexClose(-1, 1), -3);
}

TEST_F(GridCalculatorFixture, ProviderShouldRespectSizeForSameResolution) {
	auto* sizeEight = GridCalculatorProvider::get(8, 8.f);
	auto* sizeSixteen = GridCalculatorProvider::get(8, 16.f);

	EXPECT_FLOAT_EQ(sizeEight->getSize(), 8.f);
	EXPECT_FLOAT_EQ(sizeSixteen->getSize(), 16.f);
	EXPECT_FLOAT_EQ(sizeEight->getFieldSize(), 1.f);
	EXPECT_FLOAT_EQ(sizeSixteen->getFieldSize(), 2.f);
}

