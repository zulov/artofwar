#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include "utils.h"

class FibHeap
{
public:

	class FibNode
	{
	public:
		FibNode(const double k, const int pl): key(k), left(nullptr), right(nullptr), child(nullptr), degree(-1),
			payload(pl) {
		}

		~FibNode() {
		}

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

		double key;
		FibNode* left;
		FibNode* right;
		FibNode* child;
		short degree;
		int payload;
		int id;
	};

	FibHeap(): n(0), coef(
	                      1 / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)), minNode(nullptr) {
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

	FibNode* getNode(const int pl, const double k) {
		for (int i = lowestFree; i < pool.size(); ++i) {
			const auto fibNode = pool[i];
			if (fibNode->isEmpty()) {
				fibNode->payload = pl;
				fibNode->key = k;
				lowestFree = i + 1;
				return fibNode;
			}
		}
		FibNode* newNode = new FibNode(-1, -1);
		newNode->payload = pl;
		newNode->key = k;
		newNode->id = pool.size() - 1;
		pool.push_back(newNode);
		lowestFree = pool.size() - 1;
		return newNode;
	}

	void resetNode(FibNode* node) {
		if (lowestFree > node->id) {
			lowestFree = node->id;
		}
		node->reset();
	}

	void clear() {
		for (auto fibNode : pool) {
			fibNode->reset();
		}
		lowestFree = 0;
		n = 0;
		minNode = nullptr;
		std::fill_n(temp.begin(), temp.size(), nullptr);
	}

	FibNode* minimum() const {
		return minNode;
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
		const int max_degree = static_cast<int>(floor(log(static_cast<double>(n)) * coef)) + 2;
		// plus two both for indexing to max degree and so A[max_degree+1] == NIL

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
		const int toReturn = minimum()->payload;
		pop();
		return toReturn;
	}

	void put(const int pl, const double k) {
		FibNode* x = getNode(pl, k);
		x->degree = 0;
		//x->child = nullptr;
		if (minNode == nullptr) {
			minNode = x->left = x->right = x;
		}
		else {
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

	unsigned int size() {
		return static_cast<unsigned int>(n);
	}

	int n;
	double coef;
	FibNode* minNode;
	std::vector<FibNode*> temp;
	std::vector<FibNode*> pool;
	int lowestFree = 0;
};
