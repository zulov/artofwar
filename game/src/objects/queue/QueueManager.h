#pragma once
#include <vector>

enum class QueueActionType : char;
class QueueElement;

class QueueManager {
public:
	QueueManager() = default;
	~QueueManager();

	void add(short number, QueueActionType type, short id);
	QueueElement* update();
	short getSize() const;
	QueueElement* getAt(short i) const;
	QueueElement* first() const;
	void changeMaxUnitsGroupSize(unsigned char maxUnitsGroupSize);

private:
	std::vector<QueueElement*> queue;
	unsigned char maxUnitsGroup;
};
