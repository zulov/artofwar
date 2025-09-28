#pragma once
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

	const std::vector<Physical*>& getContent() const { return content; }

	//const std::span<Unit*>& getContentAsUnit() const {
	//	return std::span((Unit**)content.data(), content.size());
	//}

	void add(Physical* entity);
	void add(const std::vector<Physical*>& things);
	void remove(Physical* entity);
private:
	std::vector<Physical*> content;
};
