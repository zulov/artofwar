#pragma once
#include "defines.h"
#include <vector>


struct content_info;
class Physical;

class Bucket
{
public:
	Bucket();
	~Bucket();

	std::vector<Physical*>& getContent() { return content; }
	void add(Physical* entity);
	void remove(Physical* entity);

	bool incUnitsPerPlayer(content_info* ci, int activePlayer, const bool checks[]);
private:
	std::vector<Physical*> content;
};
