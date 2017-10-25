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

#include <cstddef>
#include <math.h>
#include <limits>
#include <iostream>

template <class T>
class FibHeap
{
public:

	class FibNode
	{
	public:
		FibNode(T k, int pl)
			: key(k), mark(false), p(nullptr), left(nullptr), right(nullptr), child(nullptr), degree(-1), payload(pl) {
		}

		~FibNode() {
		}

		T key;
		bool mark;
		FibNode* p;
		FibNode* left;
		FibNode* right;
		FibNode* child;
		int degree;
		int payload;
	};

	FibHeap()
		: n(0), min(nullptr) {
	}

	~FibHeap() {
		delete_fibnodes(min);
	}

	void delete_fibnodes(FibNode* x) {
		if (!x) {
			return;
		}

		FibNode* cur = x;
		while (true) {

			if (cur->left && cur->left != x) {
				FibNode* tmp = cur;
				cur = cur->left;
				if (tmp->child)
					delete_fibnodes(tmp->child);
				delete tmp;
			} else {
				if (cur->child)
					delete_fibnodes(cur->child);
				delete cur;
				break;
			}
		}
	}

	void insert(FibNode* x) {
		x->degree = 0;
		x->p = nullptr;
		x->child = nullptr;
		x->mark = false;
		if (min == nullptr) {
			min = x->left = x->right = x;
		} else {
			min->left->right = x;
			x->left = min->left;
			min->left = x;
			x->right = min;
			if (x->key < min->key) {
				min = x;
			}
		}
		++n;
	}

	FibNode* minimum() {
		return min;
	}

	static FibHeap* union_fibheap(FibHeap* H1, FibHeap* H2) {
		FibHeap* H = new FibHeap();
		H->min = H1->min;
		if (H->min != nullptr && H2->min != nullptr) {
			H->min->right->left = H2->min->left;
			H2->min->left->right = H->min->right;
			H->min->right = H2->min;
			H2->min->left = H->min;
		}
		if (H1->min == nullptr || (H2->min != nullptr && H2->min->key < H1->min->key)) {
			H->min = H2->min;
		}
		H->n = H1->n + H2->n;
		return H;
	}

	FibNode* extract_min() {
		FibNode *z, *x, *next;
		FibNode** childList;

		z = min;
		if (z != nullptr) {
			x = z->child;
			if (x != nullptr) {
				childList = new FibNode*[z->degree];
				next = x;
				for (int i = 0; i < (int)z->degree; i++) {
					childList[i] = next;
					next = next->right;
				}
				for (int i = 0; i < (int)z->degree; i++) {
					x = childList[i];
					min->left->right = x;
					x->left = min->left;
					min->left = x;
					x->right = min;
					x->p = nullptr;
				}
				delete [] childList;
			}
			z->left->right = z->right;
			z->right->left = z->left;
			if (z == z->right) {
				min = nullptr;
			} else {
				min = z->right;
				consolidate();
			}
			n--;
		}
		return z;
	}


	void consolidate() {
		FibNode *w, *next, *x, *y, *temp;
		FibNode **A, **rootList;
		int d, rootSize;
		int max_degree = static_cast<int>(floor(log(static_cast<double>(n)) /
		                                        log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)));

		A = new FibNode*[max_degree + 2]; // plus two both for indexing to max degree and so A[max_degree+1] == NIL
		std::fill_n(A, max_degree + 2, nullptr);
		w = min;
		rootSize = 0;
		next = w;
		do {
			rootSize++;
			next = next->right;
		} while (next != w);
		rootList = new FibNode*[rootSize];
		for (int i = 0; i < rootSize; i++) {
			rootList[i] = next;
			next = next->right;
		}
		for (int i = 0; i < rootSize; i++) {
			w = rootList[i];

			x = w;
			d = x->degree;
			while (A[d] != nullptr) {
				y = A[d];
				if (x->key > y->key) {
					temp = x;
					x = y;
					y = temp;
				}
				fib_heap_link(y, x);
				A[d] = nullptr;
				d++;
			}
			A[d] = x;
		}
		delete [] rootList;
		min = nullptr;
		for (int i = 0; i < max_degree + 2; i++) {
			if (A[i] != nullptr) {
				if (min == nullptr) {
					min = A[i]->left = A[i]->right = A[i];
				} else {
					min->left->right = A[i];
					A[i]->left = min->left;
					min->left = A[i];
					A[i]->right = min;

					if (A[i]->key < min->key) {
						min = A[i];
					}
				}
			}
		}
		delete [] A;
	}

	void fib_heap_link(FibNode* y, FibNode* x) {
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
		y->p = x;
		x->degree++;
		y->mark = false;
	}

	void decrease_key(FibNode* x, int k) {
		FibNode* y;

		if (k > x->key) {
			// error( "new key is greater than current key" );
			return;
		}
		x->key = k;
		y = x->p;
		if (y != nullptr && x->key < y->key) {
			cut(x, y);
			cascading_cut(y);
		}
		if (x->key < min->key) {
			min = x;
		}
	}

	void cut(FibNode* x, FibNode* y) {
		// 1
		if (x->right == x) {
			y->child = nullptr;
		} else {
			x->right->left = x->left;
			x->left->right = x->right;
			if (y->child == x) {
				y->child = x->right;
			}
		}
		y->degree--;
		min->right->left = x;
		x->right = min->right;
		min->right = x;
		x->left = min;
		x->p = nullptr;
		x->mark = false;
	}

	void cascading_cut(FibNode* y) {
		FibNode* z;

		z = y->p;
		if (z != nullptr) {
			if (y->mark == false) {
				y->mark = true;
			} else {
				cut(y, z);
				cascading_cut(z);
			}
		}
	}

	void remove_fibnode(FibNode* x) {
		decrease_key(x, std::numeric_limits<T>::min());
		FibNode* fn = extract_min();
		delete fn;
	}

	bool empty() const {
		return n == 0;
	}

	FibNode* topNode() {
		return minimum();
	}

	T top() {
		return minimum()->key;
	}

	void pop() {
		if (empty())
			return;
		FibNode* x = extract_min();
		if (x)
			delete x;
	}

	int get() {
		FibNode* top = topNode();
		int toReturn = top->payload;
		pop();
		return toReturn;
	}

	FibNode* put(int pl, T k) {
		FibNode* x = new FibNode(k, pl);
		insert(x);
		return x;
	}

	FibNode* put(T k) {
		return put(k, nullptr);
	}

	unsigned int size() {
		return (unsigned int)n;
	}

	int n;
	FibNode* min;
};
