#include "pch.h"

#include "env/path/PriorityHeap.h"

class PriorityHeapFixture : public ::testing::Test {};

TEST_F(PriorityHeapFixture, ReturnsIdsInAscendingPriorityOrder) {
	PriorityHeap heap;

	heap.put(10, 5);
	heap.put(20, 1);
	heap.put(30, 3);

	EXPECT_FALSE(heap.empty());
	EXPECT_EQ(heap.get(), 20);
	EXPECT_EQ(heap.get(), 30);
	EXPECT_EQ(heap.get(), 10);
	EXPECT_TRUE(heap.empty());
}

TEST_F(PriorityHeapFixture, ClearRemovesQueuedItems) {
	PriorityHeap heap;

	heap.put(1, 10);
	heap.put(2, 20);
	heap.clear();

	EXPECT_TRUE(heap.empty());

	heap.put(3, 1);
	EXPECT_EQ(heap.get(), 3);
	EXPECT_TRUE(heap.empty());
}

TEST_F(PriorityHeapFixture, HandlesNegativePriorities) {
	PriorityHeap heap;

	heap.put(7, 0);
	heap.put(8, -5);
	heap.put(9, -1);

	EXPECT_EQ(heap.get(), 8);
	EXPECT_EQ(heap.get(), 9);
	EXPECT_EQ(heap.get(), 7);
}
