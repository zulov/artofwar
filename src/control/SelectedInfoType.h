#pragma once
#include <unordered_set>
#include <vector>

class Physical;

class SelectedInfoType {
public:
	SelectedInfoType();
	~SelectedInfoType() = default;
	void clear();
	void add(Physical* physical);

	const std::vector<Physical*>& getData() const { return data; }
	const std::unordered_set<char>& getLevels() const { return levels; }
	int getId() const { return id; }
private:
	int id;
	std::unordered_set<char> levels;
	std::vector<Physical*> data;
};
