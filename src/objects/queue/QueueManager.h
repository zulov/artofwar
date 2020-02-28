#pragma once
#include <vector>

enum class QueueActionType : char;
class QueueElement;

class QueueManager {
public:
	explicit QueueManager(short maxCapacity);
	~QueueManager();

	void add(short number, QueueActionType type, short id, short localMaxCapacity);
	QueueElement* update(float time);
	short getSize() const;
	QueueElement* getAt(short i);
private:
	static float getSecToComplete(QueueActionType type, short id, int level);
	float getSecPerInstance(QueueActionType type, short id, int level);

	std::vector<QueueElement*> queue;

	short maxCapacity;
};
