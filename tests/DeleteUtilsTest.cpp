#include "pch.h"

#include <array>

#include "utils/DeleteUtils.h"

namespace {
struct TrackedDelete {
	inline static int destructed = 0;

	TrackedDelete() = default;
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
