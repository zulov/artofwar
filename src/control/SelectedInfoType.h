#pragma once
#include "objects/Physical.h"

class SelectedInfoType
{
public:
	SelectedInfoType();
	~SelectedInfoType();
	void clear();
	void add(Physical* physical);
	std::vector<Physical*>& getData();
	int getId();
private:
	int id;
	std::vector<Physical*> data;
};

