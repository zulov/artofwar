#pragma once
#include <vector>

enum class QueueActionType : char;
class QueueElement;

class QueueManager {
public:
	QueueManager() = default;
	~QueueManager();

	void add(QueueActionType type, short id, short levelId, short number = 1);
	QueueElement* update();
	short getSize() const;
	bool isEmpty() const { return getSize() == 0; }
	QueueElement* getAt(short i) const;
	QueueElement* first() const;
	void changeMaxUnitsGroupSize(unsigned char maxUnitsGroupSize);

private:
	std::vector<QueueElement*> queue;
	unsigned char maxUnitsGroup;
};
