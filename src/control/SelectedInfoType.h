#pragma once
#include <vector>

class Physical;

class SelectedInfoType
{
public:
	SelectedInfoType();
	~SelectedInfoType();
	void clear();
	void add(Physical* physical);
	
	std::vector<Physical*>& getData() { return data; }
	int getId() const { return id; }
private:
	int id;
	std::vector<Physical*> data;
};
