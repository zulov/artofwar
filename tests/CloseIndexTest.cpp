#include "pch.h"

#include "../game/src/env/CloseIndexes.h"
#include "../game/src/env/CloseIndexes.cpp"

class CloseIndexFixture : public ::testing::Test {
public:
	static CloseIndexes* ci;

protected:
	static void SetUpTestCase() {
		// Kod uruchamiany przed pierwszym testem wewnatrz `Test Case`
		short res = 8;
		ci = new CloseIndexes(res);
	}

	static void TearDownTestCase() {
		delete ci;
		// Kod uruchamiany po ostatnim tescie wewnatrz `Test Case`
	}

	static std::vector<short> param(std::vector<short> p) { return p; }
};

CloseIndexes* CloseIndexFixture::ci = nullptr;

TEST_F(CloseIndexFixture, CheckGetLv1) {
	EXPECT_EQ(ci->getLv1(0), param({ 1,8,9 }));
	EXPECT_EQ(ci->getLv1(1), param({ -1,1,7,8,9 }));
	EXPECT_EQ(ci->getLv1(6), param({ -1,1,7,8,9 }));
	EXPECT_EQ(ci->getLv1(7), param({ -1,7,8 }));
	EXPECT_EQ(ci->getLv1(8), param({ -8,-7,1,8,9 }));
	EXPECT_EQ(ci->getLv1(15), param({ -9,-8,-1,7,8 }));
	EXPECT_EQ(ci->getLv1(27), param({ -9,-8,-7,-1,1,7,8,9 }));
	EXPECT_EQ(ci->getLv1(56), param({ -8,-7,1 }));
	EXPECT_EQ(ci->getLv1(63), param({ -9,-8,-1 }));
}

TEST_F(CloseIndexFixture, CheckGetLv2) {
	EXPECT_EQ(ci->getLv2(0), param({ 2,10,16,17,18 }));
	EXPECT_EQ(ci->getLv2(1), param({ 2,10,15,16,17,18 }));
	EXPECT_EQ(ci->getLv2(2), param({ -2,2,6,10,14, 15,16,17,18 }));

	EXPECT_EQ(ci->getLv2(6), param({ -2,6,14,15,16,17 }));
	EXPECT_EQ(ci->getLv2(7), param({ -2,6,14,15,16 }));

	EXPECT_EQ(ci->getLv2(8), param({ -6,2,10,16,17,18 }));
	EXPECT_EQ(ci->getLv2(9), param({ -6,2,10,15,16,17,18 }));
	EXPECT_EQ(ci->getLv2(10), param({ -10,-6,-2,2,6,10,14,15,16,17,18 }));

	EXPECT_EQ(ci->getLv2(14), param({ -10,-2,6,14,15,16,17 }));
	EXPECT_EQ(ci->getLv2(15), param({ -10,-2,6,14,15,16 }));

	EXPECT_EQ(ci->getLv2(16), param({ -16,-15,-14,-6,2,10,16,17,18 }));
	EXPECT_EQ(ci->getLv2(17), param({ -17,-16,-15,-14,-6,2,10,15, 16,17,18 }));
	EXPECT_EQ(ci->getLv2(18), param({ -18,-17,-16,-15,-14,-10,-6,-2,2,6,10,14,15,16,17,18 }));

	EXPECT_EQ(ci->getLv2(22), param({ -18,-17,-16,-15,-10,-2,6,14,15,16,17 }));
	EXPECT_EQ(ci->getLv2(23), param({ -18,-17,-16,-10,-2,6,14,15,16 }));

	EXPECT_EQ(ci->getLv2(48), param({ -16,-15,-14,-6,2,10 }));
	EXPECT_EQ(ci->getLv2(49), param({ -17,-16,-15,-14,-6,2,10 }));
	EXPECT_EQ(ci->getLv2(50), param({ -18,-17,-16,-15,-14,-10,-6,-2,2,6,10 }));

	EXPECT_EQ(ci->getLv2(54), param({ -18,-17,-16,-15,-10,-2,6 }));
	EXPECT_EQ(ci->getLv2(55), param({ -18,-17,-16,-10,-2,6 }));

	EXPECT_EQ(ci->getLv2(56), param({ -16,-15,-14,-6,2 }));
	EXPECT_EQ(ci->getLv2(57), param({ -17,-16,-15,-14,-6,2 }));
	EXPECT_EQ(ci->getLv2(58), param({ -18,-17,-16,-15,-14,-10,-6,-2,2 }));

	EXPECT_EQ(ci->getLv2(62), param({ -18,-17,-16,-15,-10,-2 }));
	EXPECT_EQ(ci->getLv2(63), param({ -18,-17,-16,-10,-2 }));
}


TEST_F(CloseIndexFixture, CheckBothIndex) {
	EXPECT_EQ(ci->getBothIndexes(0), std::pair(0,0));
	EXPECT_EQ(ci->getBothIndexes(1), std::pair(1,1));
	EXPECT_EQ(ci->getBothIndexes(2), std::pair(1,2));
	EXPECT_EQ(ci->getBothIndexes(6), std::pair(1,3));
	EXPECT_EQ(ci->getBothIndexes(7), std::pair(2,4));

	EXPECT_EQ(ci->getBothIndexes(27), std::pair(4,12));

	EXPECT_EQ(ci->getBothIndexes(54), std::pair(4,18));
	EXPECT_EQ(ci->getBothIndexes(55), std::pair(5,19));

	EXPECT_EQ(ci->getBothIndexes(56), std::pair(6, 20));
	EXPECT_EQ(ci->getBothIndexes(57), std::pair(7, 21));
	EXPECT_EQ(ci->getBothIndexes(58), std::pair(7, 22));

	EXPECT_EQ(ci->getBothIndexes(62), std::pair(7,23));
	EXPECT_EQ(ci->getBothIndexes(63), std::pair(8,24));
}