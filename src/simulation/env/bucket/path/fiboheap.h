#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "utils/DeleteUtils.h"

constexpr short CACHE_SIZE = 8192;

static unsigned char degree_cache[CACHE_SIZE];

class FibHeap {
public:
	class FibNode {
	public:
		FibNode(const float k, const int pl, int id):
			left(nullptr), right(nullptr), child(nullptr),
			key(k), payload(pl), id(id), degree(0) { }

		~FibNode() = default;

		bool isEmpty() const {
			return key < 0.f;
		}

		void reset() {
			key = -1.f;
			left = nullptr;
			right = nullptr;
			child = nullptr;
			degree = 0;
			payload = -1;
		}

		FibNode* left;
		FibNode* right;
		FibNode* child;
		float key;
		int payload;
		int id;
		unsigned short degree;
	};

	FibHeap() {
		temp.resize(15, nullptr);
		pool.resize(400, nullptr);
		for (auto i = 0; i < 400; ++i) {
			pool[i] = new FibNode(-1, -1, i);
		}
	}

	~FibHeap() {
		clear_vector(pool);
	}

	FibNode* getNode(const int pl, const float k) {
		for (auto i = lowestFree; i < pool.size(); ++i) {
			auto* const fibNode = pool[i];
			if (fibNode->isEmpty()) {
				fibNode->payload = pl;
				fibNode->key = k;
				if (i > highestUsed) {
					highestUsed = i;
				}
				lowestFree = i + 1;
				return fibNode;
			}
		}
		auto* const newNode = new FibNode(k, pl, pool.size() - 1);

		pool.push_back(newNode);

		lowestFree = pool.size() - 1;
		if (lowestFree > highestUsed) {
			highestUsed = lowestFree;
		}
		return newNode;
	}

	void resetNode(FibNode* node) {
		if (lowestFree > node->id) {
			lowestFree = node->id;
		}
		node->reset();
	}

	void clear() {
		for (auto i = 0; i <= highestUsed; ++i) {
			pool[i]->reset();
		}

		lowestFree = 0;
		highestUsed = 0;
		n = 0;
		minNode = nullptr;
		std::fill_n(temp.begin(), temp.size(), nullptr);
	}

	void ensureSizeAndClear(unsigned int size) {
		if (temp.size() < size) {
			temp.resize(size);
		}

		std::fill_n(temp.begin(), size, nullptr);
	}

	FibNode* extract_min() {
		const auto z = minNode;
		if (z != nullptr) {
			auto x = z->child;
			if (x != nullptr) {
				ensureSizeAndClear(z->degree);

				auto next = x;
				for (auto i = 0; i < z->degree; ++i) {
					temp[i] = next;
					next = next->right;
				}
				for (auto i = 0; i < z->degree; ++i) {
					x = temp[i];
					minNode->left->right = x;
					x->left = minNode->left;
					minNode->left = x;
					x->right = minNode;
				}
			}
			z->left->right = z->right;
			z->right->left = z->left;
			if (z == z->right) {
				minNode = nullptr;
			} else {
				minNode = z->right;
				consolidate();
			}
			--n;
		}
		return z;
	}

	void consolidate() {
		assert(n<CACHE_SIZE);
		if (n >= CACHE_SIZE) {
			std::cout << "DUPA" << std::endl;
		}

		auto w = minNode;
		unsigned short rootSize = 0;
		auto next = w;
		do {
			++rootSize;
			next = next->right;
		} while (next != w);

		const auto max_degree = degree_cache[n];

		const auto secondSize = max_degree + rootSize;
		ensureSizeAndClear(secondSize);

		for (int i = max_degree; i < secondSize; ++i) {
			temp[i] = next;
			next = next->right;
		}
		for (int i = max_degree; i < secondSize; ++i) {
			w = temp[i];

			auto x = w;
			int d = x->degree;
			while (temp[d] != nullptr) {
				auto y = temp[d];
				if (x->key > y->key) {
					std::swap(x, y);
				}
				fib_heap_link(y, x);
				temp[d] = nullptr;
				++d;
			}
			temp[d] = x;
		}

		minNode = nullptr;
		for (auto i = 0; i < max_degree; ++i) {
			if (temp[i] != nullptr) {
				if (minNode == nullptr) {
					minNode = temp[i]->left = temp[i]->right = temp[i];
				} else {
					minNode->left->right = temp[i];
					temp[i]->left = minNode->left;
					minNode->left = temp[i];
					temp[i]->right = minNode;

					if (temp[i]->key < minNode->key) {
						minNode = temp[i];
					}
				}
			}
		}
	}

	static void fib_heap_link(FibNode* y, FibNode* x) {
		y->left->right = y->right;
		y->right->left = y->left;
		if (x->child != nullptr) {
			x->child->left->right = y;
			y->left = x->child->left;
			x->child->left = y;
			y->right = x->child;
		} else {
			x->child = y;
			y->right = y;
			y->left = y;
		}
		++x->degree;
		assert(x->degree<60000);
	}

	bool empty() const {
		return n == 0;
	}

	void pop() {
		if (empty()) {
			return;
		}
		const auto x = extract_min();
		if (x) {
			resetNode(x);
		}
	}

	int get() {
		const auto toReturn = minNode->payload;
		pop();
		return toReturn;
	}

	void put(const int pl, const float k) {
		const auto x = getNode(pl, k);
		x->degree = 0;
		//x->child = nullptr;
		if (minNode == nullptr) {
			minNode = x->left = x->right = x;
		} else {
			minNode->left->right = x;
			x->left = minNode->left;
			minNode->left = x;
			x->right = minNode;
			if (x->key < minNode->key) {
				minNode = x;
			}
		}
		++n;
	}

	FibNode* minNode{nullptr};
	std::vector<FibNode*> temp;
	std::vector<FibNode*> pool;

	int n{0};
	int lowestFree = 0;
	int highestUsed = 399;

	static void initCache() {
		const auto coef = 1 / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2);
		for (auto i = 0; i < CACHE_SIZE; ++i) {
			degree_cache[i] = static_cast<unsigned char>(floor(log(static_cast<double>(i)) * coef)) + 2;
			// plus two both for indexing to max degree and so A[max_degree+1] == NIL 
		}
	}
};
