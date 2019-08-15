#pragma once
#include <vector>


struct content_info;
class Physical;

class Bucket {
public:
	Bucket();
	~Bucket();
	Bucket(const Bucket&) = delete;

	std::vector<Physical*>& getContent() { return content; }
	void add(Physical* entity);
	void remove(Physical* entity);
private:
	std::vector<Physical*> content;
};
