#pragma once
#include <vector>
#include <array>

class Physical;

class SelectedInfoType {
public:
	explicit SelectedInfoType(unsigned short id);
	~SelectedInfoType() = default;
	void clear();
	void add(Physical* physical);

	const std::vector<Physical*>& getData() const { return data; }
	unsigned short getId() const { return id; }
private:
	unsigned short id;
	std::vector<Physical*> data;
};
