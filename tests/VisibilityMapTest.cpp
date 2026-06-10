#include "pch.h"

#include <vector>
#include <numeric>

#include "math/MathUtils.h"

// These tests lock down the index math used by VisibilityMap::ensureReady, which
// downsamples a full-resolution visibility grid (side = visibilityRes) into a
// half-resolution influence mask (side = influenceRes = visibilityRes / 2).
//
// The optimization replaced a linear scan that called getCordsInLower(j) per cell
// (which uses j / visibilityRes and j % visibilityRes -- a runtime-divisor div+mod)
// with a nested (row, col) walk that derives the child index by halving each
// coordinate. These tests prove the two formulations are equivalent, so the refactor
// cannot have changed which influence cells get marked.

namespace {

// Reference downsampling: the ORIGINAL formulation (linear j + getCordsInLower).
std::vector<bool> downsampleReference(int visibilityRes, const std::vector<bool>& visible) {
	const int influenceRes = visibilityRes / 2;
	std::vector<bool> mask(influenceRes * influenceRes, false);
	const int n = visibilityRes * visibilityRes;
	for (int j = 0; j < n; ++j) {
		if (visible[j]) {
			mask[getCordsInLower(influenceRes, visibilityRes, j)] = true;
		}
	}
	return mask;
}

// New downsampling: EXACTLY mirrors the loop in VisibilityMap::ensureReady.
std::vector<bool> downsampleNested(int visibilityRes, const std::vector<bool>& visible) {
	const int influenceRes = visibilityRes / 2;
	std::vector<bool> mask(influenceRes * influenceRes, false);
	int j = 0;
	for (int prow = 0; prow < visibilityRes; ++prow) {
		const int rowBase = (prow / 2) * influenceRes;
		for (int pcol = 0; pcol < visibilityRes; ++pcol, ++j) {
			if (visible[j]) {
				const int newIndex = rowBase + (pcol / 2);
				mask[newIndex] = true;
			}
		}
	}
	return mask;
}

} // namespace

class VisibilityDownsampleFixture : public ::testing::Test {};

// Single visible cell: both formulations agree, exhaustively, for every parent index.
TEST_F(VisibilityDownsampleFixture, SingleCellEquivalentExhaustive) {
	for (int res : { 2, 4, 8, 16, 32 }) {
		const int n = res * res;
		for (int j = 0; j < n; ++j) {
			std::vector<bool> visible(n, false);
			visible[j] = true;
			EXPECT_EQ(downsampleNested(res, visible), downsampleReference(res, visible))
				<< "res=" << res << " visibleIndex=" << j;
		}
	}
}

// The mapping must match getCordsInLower itself for a single cell (one bit set).
TEST_F(VisibilityDownsampleFixture, SingleCellMatchesGetCordsInLower) {
	constexpr int RES = 8;
	const int n = RES * RES;
	const int influenceRes = RES / 2;
	for (int j = 0; j < n; ++j) {
		std::vector<bool> visible(n, false);
		visible[j] = true;
		const auto mask = downsampleNested(RES, visible);

		const int expectedIdx = getCordsInLower(influenceRes, RES, j);
		for (int k = 0; k < influenceRes * influenceRes; ++k) {
			EXPECT_EQ(mask[k], k == expectedIdx)
				<< "res=" << RES << " parentIndex=" << j << " influenceCell=" << k;
		}
	}
}

// Empty input -> empty mask.
TEST_F(VisibilityDownsampleFixture, EmptyProducesAllFalse) {
	constexpr int RES = 8;
	std::vector<bool> visible(RES * RES, false);

	const auto mask = downsampleNested(RES, visible);
	ASSERT_EQ(mask.size(), static_cast<size_t>((RES / 2) * (RES / 2)));
	for (bool b : mask) {
		EXPECT_FALSE(b);
	}
	EXPECT_EQ(mask, downsampleReference(RES, visible));
}

// All cells visible -> every influence cell true (each maps from a 2x2 parent block).
TEST_F(VisibilityDownsampleFixture, AllVisibleProducesAllTrue) {
	constexpr int RES = 8;
	std::vector<bool> visible(RES * RES, true);

	const auto mask = downsampleNested(RES, visible);
	for (bool b : mask) {
		EXPECT_TRUE(b);
	}
	EXPECT_EQ(mask, downsampleReference(RES, visible));
}

// Scattered cells (including all four corners and interior) agree with the reference.
TEST_F(VisibilityDownsampleFixture, ScatteredCellsEquivalent) {
	constexpr int RES = 8;
	const int n = RES * RES;
	std::vector<bool> visible(n, false);
	for (int j : { 0, 1, 7, 8, 9, 18, 27, 36, 45, 54, 56, 63 }) {
		visible[j] = true;
	}

	EXPECT_EQ(downsampleNested(RES, visible), downsampleReference(RES, visible));
}

// Each 2x2 parent block maps to a single influence cell: any one of the four parent
// cells being visible sets the same influence bit.
TEST_F(VisibilityDownsampleFixture, EachParentBlockMapsToOneInfluenceCell) {
	constexpr int RES = 8;
	const int n = RES * RES;
	const int influenceRes = RES / 2;

	for (int br = 0; br < influenceRes; ++br) {
		for (int bc = 0; bc < influenceRes; ++bc) {
			const int expectedIdx = br * influenceRes + bc;
			// The four parent cells of this block.
			const int p00 = (br * 2) * RES + (bc * 2);
			const int corners[4] = { p00, p00 + 1, p00 + RES, p00 + RES + 1 };
			for (int corner : corners) {
				std::vector<bool> visible(n, false);
				visible[corner] = true;
				const auto mask = downsampleNested(RES, visible);
				for (int k = 0; k < influenceRes * influenceRes; ++k) {
					EXPECT_EQ(mask[k], k == expectedIdx)
						<< "block(" << br << "," << bc << ") parent=" << corner << " cell=" << k;
				}
			}
		}
	}
}
