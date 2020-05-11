#pragma once

#include "utils/DeleteUtils.h"
#include <cmath>
#include <vector>

static int degree_cache[1000];

class FibHeap {
public:

	class FibNode {
	public:
		FibNode(const float k, const int pl): left(nullptr), right(nullptr), child(nullptr), key(k), payload(pl),
		                                      degree(-1) {
		}

		~FibNode() = default;

		bool isEmpty() const {
			return key < 0;
		}

		void reset() {
			key = -1;
			left = nullptr;
			right = nullptr;
			child = nullptr;
			degree = -1;
			payload = -1;
		}


		FibNode* left;
		FibNode* right;
		FibNode* child;
		float key;
		int payload;
		int id;
		short degree;

	};

	FibHeap() {
		temp.resize(15, nullptr);
		pool.resize(400, nullptr);
		for (int i = 0; i < 400; ++i) {
			pool[i] = new FibNode(-1, -1);
			pool[i]->id = i;
		}

	}

	~FibHeap() {
		clear_vector(pool);
	}


	FibNode* getNode(const int pl, const float k) {
		for (int i = lowestFree; i < pool.size(); ++i) {
			const auto fibNode = pool[i];
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
		const auto newNode = new FibNode(k, pl);
		newNode->id = pool.size() - 1;
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
		for (int i = 0; i <= highestUsed; ++i) {
			pool[i]->reset();
		}

		lowestFree = 0;
		highestUsed = 0;
		n = 0;
		minNode = nullptr;
		std::fill_n(temp.begin(), temp.size(), nullptr);
	}

	FibNode* extract_min() {
		FibNode* z = minNode;
		if (z != nullptr) {
			FibNode* x = z->child;
			if (x != nullptr) {

				if (temp.size() < z->degree) {
					temp.resize(z->degree);
				}

				std::fill_n(temp.begin(), z->degree, nullptr);

				FibNode* next = x;
				for (int i = 0; i < z->degree; ++i) {
					temp[i] = next;
					next = next->right;
				}
				for (int i = 0; i < z->degree; ++i) {
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
		int max_degree = degree_cache[n];

		if (temp.size() < max_degree) {
			temp.resize(max_degree);
		}

		std::fill_n(temp.begin(), max_degree, nullptr);

		FibNode* w = minNode;
		int rootSize = 0;
		FibNode* next = w;
		do {
			++rootSize;
			next = next->right;
		} while (next != w);
		const int secondSize = max_degree + rootSize;
		if (temp.size() < secondSize) {
			temp.resize(secondSize);
		}
		fill_n(temp.begin() + max_degree, rootSize, nullptr);

		for (int i = max_degree; i < secondSize; ++i) {
			temp[i] = next;
			next = next->right;
		}
		for (int i = max_degree; i < secondSize; ++i) {
			w = temp[i];

			FibNode* x = w;
			int d = x->degree;
			while (temp[d] != nullptr) {
				FibNode* y = temp[d];
				if (x->key > y->key) {
					FibNode* temp = x;
					x = y;
					y = temp;
				}
				fib_heap_link(y, x);
				temp[d] = nullptr;
				++d;
			}
			temp[d] = x;
		}

		minNode = nullptr;
		for (int i = 0; i < max_degree; ++i) {
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
	}

	bool empty() const {
		return n == 0;
	}

	void pop() {
		if (empty()) {
			return;
		}
		FibNode* x = extract_min();
		if (x) {
			resetNode(x);
		}
	}

	int get() {
		const int toReturn = minNode->payload;
		pop();
		return toReturn;
	}

	void put(const int pl, const float k) {
		FibNode* x = getNode(pl, k);
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
		double coef = 1 / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2);
		for (int i = 0; i < 1000; ++i) {
			degree_cache[i] = static_cast<int>(floor(log(static_cast<double>(i)) * coef)) + 2;
			// plus two both for indexing to max degree and so A[max_degree+1] == NIL 
		}
	}
};
