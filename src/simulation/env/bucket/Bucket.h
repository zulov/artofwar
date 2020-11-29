#pragma once
#include <vector>

struct content_info;
class Physical;

class Bucket {
public:
	Bucket() = default;
	~Bucket() = default;
	int getSize() const;
	Bucket(const Bucket&) = delete;

	const std::vector<Physical*>& getContent() const { return content; }
	void add(Physical* entity);
	void remove(Physical* entity);
private:
	std::vector<Physical*> content;
};
