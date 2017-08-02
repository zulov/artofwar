#pragma once
#include "Physical.h"

class SelectedInfoType
{
public:
	SelectedInfoType();
	~SelectedInfoType();
	void clear();
	void add(Physical* physical);
	std::vector<Physical*>* getData();
private:
	std::vector<Physical*>* data;
};

