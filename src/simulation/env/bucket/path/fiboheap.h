#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "utils/DeleteUtils.h"

constexpr short CACHE_SIZE = 8192;

static unsigned char DEGREE_CACHE[CACHE_SIZE];

class FibHeap {
public:
	class FibNode {
	public:
		FibNode(const float k, const int pl):
			left(nullptr), right(nullptr), child(nullptr),
			key(k), payload(pl), degree(0) { }

		FibNode(const FibNode& obj) = delete;
		~FibNode() = default;

		bool isEmpty() const {
			return key < 0.f;
		}

		void reset() {
			left = nullptr;
			right = nullptr;
			child = nullptr;
			key = -1.f;
			payload = -1;
			degree = 0;
		}

		FibNode* left;
		FibNode* right;
		FibNode* child;
		float key;
		int payload;
		unsigned short degree;
	};

	FibHeap(): tempSize(100) {
		temp = new FibNode*[tempSize];
		const short initialSize = 1024;
		freePool.reserve(initialSize);
		allPool.reserve(initialSize);
		for (auto i = 0; i < initialSize; ++i) {
			auto fn = new FibNode(-1, -1);
			freePool.push_back(fn);
			allPool.push_back(fn);
		}
	}

	FibHeap(const FibHeap& obj) = delete;

	~FibHeap() {
		delete[] temp;
		clear_vector(allPool);
	}

	FibNode* getNode(const int pl, const float k) {
		assert(pl >= 0);
		assert(k >= 0.f);
		if (freePool.empty()) {
			const auto fn = new FibNode(k, pl);
			allPool.push_back(fn);
			return fn;
		}
		const auto fibNode = freePool.back();
		freePool.pop_back();

		fibNode->payload = pl;
		fibNode->key = k;

		return fibNode;
	}

	void resetNode(FibNode* node) {
		if (node->key >= 0.f) {
			freePool.push_back(node);
		}
		node->reset();
	}

	void clear() {
		n = 0;
		minNode = nullptr;
		for (const auto fibNode : allPool) {
			resetNode(fibNode);
		}
		std::fill_n(temp, tempSize, nullptr);
	}

	void ensureSizeAndClear(unsigned int size) {
		if (tempSize < size) {
			tempSize = size;
			delete[] temp;
			temp = new FibNode*[tempSize];
		}

		std::fill_n(temp, tempSize, nullptr);
	}

	FibNode* extractMin() {
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

		auto w = minNode;
		unsigned short rootSize = 0;
		auto next = w;
		do {
			++rootSize;
			next = next->right;
		} while (next != w);

		const auto maxDegree = DEGREE_CACHE[n];

		const auto secondSize = maxDegree + rootSize;
		ensureSizeAndClear(secondSize);

		for (int i = maxDegree; i < secondSize; ++i) {
			temp[i] = next;
			next = next->right;
		}
		for (int i = maxDegree; i < secondSize; ++i) {
			w = temp[i];

			auto x = w;
			int d = x->degree;
			while (temp[d] != nullptr) {
				auto y = temp[d];
				if (x->key > y->key) {
					std::swap(x, y);
				}
				fibHeapLink(y, x);
				temp[d] = nullptr;
				++d;
			}
			temp[d] = x;
		}

		minNode = nullptr;
		auto tii = temp;
		for (auto i = 0; i < maxDegree; ++i, ++tii) {
			const auto ti = *tii;
			if (ti != nullptr) {
				if (minNode == nullptr) {
					minNode = ti->left = ti->right = ti;
				} else {
					minNode->left->right = ti;
					ti->left = minNode->left;
					minNode->left = ti;
					ti->right = minNode;

					if (ti->key < minNode->key) {
						minNode = ti;
					}
				}
			}
		}
	}

	static void fibHeapLink(FibNode* y, FibNode* x) {
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
		const auto x = extractMin();
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

	FibNode* minNode{};
	std::vector<FibNode*> freePool;
	std::vector<FibNode*> allPool;
	FibNode** temp;
	unsigned short tempSize;

	int n{0};

	static void initCache() {
		const auto coef = 1 / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2);
		for (auto i = 0; i < CACHE_SIZE; ++i) {
			DEGREE_CACHE[i] = static_cast<unsigned char>(floor(log(static_cast<double>(i)) * coef)) + 2;
			// plus two both for indexing to max degree and so A[max_degree+1] == NIL 
		}
	}
};
