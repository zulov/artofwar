#pragma once
template <typename T>
char cast(T type) {
	return static_cast<char>(type);
}

inline size_t key(int i, int j) { return (size_t)i << 32 | (unsigned int)j; }