#pragma once
#include <vector>

enum class QueueActionType : unsigned char;
class QueueElement;

class QueueManager {
public:
	QueueManager() = default;
	~QueueManager();

	void add(QueueActionType type, unsigned short id, unsigned short number, unsigned short durationTicks);
	QueueElement* update();
	short getSize() const;
	bool isEmpty() const { return getSize() == 0; }
	QueueElement* getAt(short i) const;
	QueueElement* first() const;
	bool contains(QueueActionType type, unsigned short id) const;
	void changeMaxUnitsGroupSize(unsigned char maxUnitsGroupSize);
	void restore(QueueActionType type, unsigned short id, unsigned short amount,
				 unsigned short elapsedTicks, unsigned short durationTicks);

private:
	std::vector<QueueElement*> queue;
	unsigned char maxUnitsGroup = 1;
};
