#pragma once
#include <vector>

enum class QueueActionType : char;
class QueueElement;

class QueueManager {
public:
	explicit QueueManager(unsigned short maxCapacity);
	~QueueManager();

	void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity);
	QueueElement* update(float time);
	short getSize() const;
	QueueElement* getAt(short i);
private:
	std::vector<QueueElement*> queue;

	unsigned short maxCapacity;
};
