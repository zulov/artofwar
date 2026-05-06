#include "pch.h"

#include <array>
#include <initializer_list>

#include "env/CloseIndexes.h"
#include "env/CloseIndexesProvider.h"
#include "env/bucket/ComplexBucketData.h"

#include "Game.cpp"
#include "env/path/PathFinder.cpp"

class PathFinderFixture : public ::testing::Test {
protected:
	static constexpr short kResolution = 8;
	static constexpr float kSize = 8.f;
	static constexpr int kCellCount = kResolution * kResolution;

	PathFinder pathFinder{kResolution, kSize};
	CloseIndexes* closeIndexes = CloseIndexesProvider::get(kResolution);
	std::array<ComplexBucketData, kCellCount> cells{};

	void SetUp() override {
		pathFinder.setComplexBucketData(cells.data());
		rebuildGrid({});
	}

	const std::vector<int>* findPath(int startIdx, int endIdx) {
		return pathFinder.findPath(startIdx, std::vector<int>{endIdx});
	}

	void rebuildGrid(std::initializer_list<int> blocked, bool invalidateCache = true) {
		std::array<bool, kCellCount> blockedMask{};
		for (const int idx : blocked) {
			ASSERT_GE(idx, 0);
			ASSERT_LT(idx, kCellCount);
			blockedMask[idx] = true;
		}

		for (int i = 0; i < kCellCount; ++i) {
			auto& cell = cells[i];
			cell.resetForReuse();
			cell.setIndexCloseIndexes(closeIndexes->getBothIndexes(i));
			if (blockedMask[i]) {
				cell.incStateSize(CellState::BUILDING);
			}
		}

		// Mirror MainGrid neighbor bookkeeping so PathFinder sees valid edge and obstacle masks.
		for (int i = 0; i < kCellCount; ++i) {
			refreshNeighbors(i);
		}

		if (invalidateCache) {
			pathFinder.invalidateCache();
		}
	}

private:
	void refreshNeighbors(int index) {
		auto& cell = cells[index];
		cell.setAllOccupied();
		for (const auto& [bit, delta] : closeIndexes->getTabIndexesWithValue(cell)) {
			if (cells[index + delta].isPassable()) {
				cell.setNeightFree(bit);
			}
		}
	}
};

TEST_F(PathFinderFixture, ReturnsAdjacentTargetWithoutRunningFullSearch) {
	const auto* path = findPath(9, 10);

	ASSERT_NE(path, nullptr);
	EXPECT_EQ(*path, std::vector<int>({10}));
}

TEST_F(PathFinderFixture, ReturnsPassableIntermediateForSecondRingTarget) {
	const auto* path = findPath(0, 16);

	ASSERT_NE(path, nullptr);
	EXPECT_EQ(*path, std::vector<int>({8, 16}));
}

TEST_F(PathFinderFixture, CachesSimplifiedPathForRepeatedQuery) {
	const auto* firstPath = findPath(9, 36);
	const auto* secondPath = findPath(9, 36);

	ASSERT_NE(firstPath, nullptr);
	ASSERT_NE(secondPath, nullptr);
	EXPECT_EQ(*firstPath, std::vector<int>({36}));
	EXPECT_EQ(*secondPath, std::vector<int>({36}));
	EXPECT_NE(firstPath, secondPath);
}

TEST_F(PathFinderFixture, InvalidateCacheForcesRecomputationAfterGridChanges) {
	const auto* cachedPath = findPath(9, 36);
	ASSERT_NE(cachedPath, nullptr);
	EXPECT_EQ(*cachedPath, std::vector<int>({36}));

	rebuildGrid({36}, false);
	pathFinder.invalidateCache();

	const auto* recomputedPath = findPath(9, 36);
	ASSERT_NE(recomputedPath, nullptr);
	EXPECT_TRUE(recomputedPath->empty());
}
