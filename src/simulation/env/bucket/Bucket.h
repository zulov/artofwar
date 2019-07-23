#pragma once
#include <vector>


struct content_info;
class Physical;

class Bucket {
public:
	Bucket();
	~Bucket();

	std::vector<Physical*>& getContent() { return content; }
	void add(Physical* entity);
	void remove(Physical* entity);
private:
	std::vector<Physical*> content;
};
