#pragma once
#include <vector>

template <class T>
class AbstractGrid {
public:
	virtual std::vector<T*>& getContentAt(int index)=0;
};
