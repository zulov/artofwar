#pragma once
#include <span>
#include <vector>

class Unit;
struct content_info;
class Physical;

class Bucket {
public:
	Bucket() = default;
	~Bucket() = default;
	Bucket(const Bucket&) = delete;

	int getSize() const;
	void reserve(int i);

	std::span<Physical* const> getContent() const { return {content.data(), content.size()}; }

	//const std::span<Unit*>& getContentAsUnit() const {
	//	return std::span((Unit**)content.data(), content.size());
	//}

	void add(Physical* entity);
	void add(std::span<Physical* const> things);
	void remove(Physical* entity);
	void clear();
	void resetForReuse() { clear(); }
private:
	std::vector<Physical*> content;
};
