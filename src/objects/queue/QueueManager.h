#pragma once
#include <vector>

enum class MenuAction : char;
class QueueElement;

class QueueManager
{
public:
	explicit QueueManager(short maxCapacity);
	~QueueManager();

	void add(short value, MenuAction type, short id, short localMaxCapacity);
	QueueElement* update(float time);
	short getSize();
	QueueElement* getAt(short i);
private:
	float getSecToComplete(MenuAction type, short id, int level);
	float getSecPerInstance(MenuAction type, short id, int level);

	std::vector<QueueElement*> queue;

	short maxCapacity;
};
