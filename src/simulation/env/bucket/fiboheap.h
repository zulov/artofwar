/**
 * Fibonacci Heap
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#pragma once

#include <math.h>

#include <iostream>
#include "utils.h"

class FibHeap
{
public:

	class FibNode
	{
	public:
		FibNode(double k, int pl): key(k), left(nullptr), right(nullptr), child(nullptr), degree(-1),
			payload(pl) {
		}

		~FibNode() {
		}

		bool isEmpty() {
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
		cached.resize(100, nullptr);
		for (int i = 0; i < 100; ++i) {
			cached[i] = new FibNode(-1, -1);
			cached[i]->id = i;
		}
	}

	~FibHeap() {
		//delete_fibnodes(minNode);
		for (auto fibNode : cached) {
			delete fibNode;
		}
	}

	FibNode* getNode(int pl, double k) {
		for (int i = lowestFree; i < cached.size(); ++i) {
			auto fibNode = cached[i];
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
		newNode->id = cached.size() - 1;
		cached.push_back(newNode);
		lowestFree = cached.size() - 1;
		return newNode;
	}

	void resetNode(FibNode* node) {
		if (lowestFree > node->id) {
			lowestFree = node->id;
		}
		node->reset();
	}

	void clear() {
		for (auto fibNode : cached) {
			fibNode->reset();
		}
		lowestFree = 0;
		n = 0;
		minNode = nullptr;
		std::fill_n(temp.begin(), temp.size(), nullptr);
	}

	void insert(FibNode* x) {
		x->degree = 0;
		x->child = nullptr;
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

	FibNode* minimum() {
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
		int secondSize = max_degree + rootSize;
		if (temp.size() < secondSize) {
			temp.resize(secondSize);
		}
		std::fill_n(temp.begin() + max_degree, rootSize, nullptr);

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

	FibNode* topNode() {
		return minimum();
	}

	double top() {
		return minimum()->key;
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
		int toReturn = topNode()->payload;
		pop();
		return toReturn;
	}

	void put(int pl, double k) {
		insert(getNode(pl, k));
	}

	void put(double k) {
		put(0, k);
	}

	unsigned int size() {
		return (unsigned int)n;
	}

	int n;
	double coef;
	FibNode* minNode;
	std::vector<FibNode*> temp;
	std::vector<FibNode*> cached;
	int lowestFree = 0;
};
