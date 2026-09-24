#include "pch.h"

#include <array>

#include "utils/DeleteUtils.h"

namespace {
struct TrackedDelete {
	inline static int destructed = 0;
	const int value;
	const bool marked;

	TrackedDelete(int value = 0, bool marked = false) : value(value), marked(marked) {}
	~TrackedDelete() {
		++destructed;
	}

	static void reset() {
		destructed = 0;
	}
};
}

class DeleteUtilsFixture : public ::testing::Test {
protected:
	void SetUp() override {
		TrackedDelete::reset();
	}
};

TEST_F(DeleteUtilsFixture, ClearVectorReferenceDeletesAllObjectsAndClearsContainer) {
	std::vector<TrackedDelete*> items = { new TrackedDelete(), new TrackedDelete() };

	clear_vector(items);

	EXPECT_TRUE(items.empty());
	EXPECT_EQ(TrackedDelete::destructed, 2);
}

TEST_F(DeleteUtilsFixture, ClearVectorPointerDeletesAllObjectsAndClearsContainer) {
	std::vector<TrackedDelete*> items = { new TrackedDelete(), new TrackedDelete(), new TrackedDelete() };

	clear_vector(&items);

	EXPECT_TRUE(items.empty());
	EXPECT_EQ(TrackedDelete::destructed, 3);
}

TEST_F(DeleteUtilsFixture, ClearAndDeleteEmptyVectorIsSafe) {
	auto* items = new std::vector<TrackedDelete*>();

	clear_and_delete_vector(items);

	EXPECT_EQ(TrackedDelete::destructed, 0);
}

TEST_F(DeleteUtilsFixture, EraseAndDeleteIfRemovesMatchesAndPreservesOrder) {
	std::vector<TrackedDelete*> items = {
		new TrackedDelete(1), new TrackedDelete(2, true), new TrackedDelete(3), new TrackedDelete(4, true)
	};

	eraseAndDeleteIf(items, [](const TrackedDelete* item) { return item->marked; });

	ASSERT_EQ(items.size(), 2);
	EXPECT_EQ(items[0]->value, 1);
	EXPECT_EQ(items[1]->value, 3);
	EXPECT_EQ(TrackedDelete::destructed, 2);
	clear_vector(items);
}

TEST_F(DeleteUtilsFixture, EraseAndDeleteIfLeavesUnmatchedItems) {
	std::vector<TrackedDelete*> items = { new TrackedDelete(1), new TrackedDelete(2) };

	eraseAndDeleteIf(items, [](const TrackedDelete* item) { return item->marked; });

	EXPECT_EQ(items.size(), 2);
	EXPECT_EQ(TrackedDelete::destructed, 0);
	clear_vector(items);
}

TEST_F(DeleteUtilsFixture, EraseAndDeleteIfRemovesAllMatches) {
	std::vector<TrackedDelete*> items = { new TrackedDelete(1, true), new TrackedDelete(2, true) };

	eraseAndDeleteIf(items, [](const TrackedDelete* item) { return item->marked; });

	EXPECT_TRUE(items.empty());
	EXPECT_EQ(TrackedDelete::destructed, 2);
}

TEST_F(DeleteUtilsFixture, ClearStdArrayDeletesAllStoredPointers) {
	std::array<TrackedDelete*, 3> items = { new TrackedDelete(), new TrackedDelete(), new TrackedDelete() };

	clear_array(items);

	EXPECT_EQ(TrackedDelete::destructed, 3);
}

TEST_F(DeleteUtilsFixture, ClearVectorArrayDeletesEveryArrayAndClearsContainer) {
	std::vector<TrackedDelete*> items;
	items.push_back(new TrackedDelete[2]);
	items.push_back(new TrackedDelete[3]);

	clear_vector_array(items);

	EXPECT_TRUE(items.empty());
	EXPECT_EQ(TrackedDelete::destructed, 5);
}

TEST_F(DeleteUtilsFixture, ClearArrayDeletesEveryElementPointer) {
	auto** items = new TrackedDelete*[2];
	items[0] = new TrackedDelete();
	items[1] = new TrackedDelete();

	clear_array(items, 2);

	EXPECT_EQ(TrackedDelete::destructed, 2);
	delete[] items;
}
