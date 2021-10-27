#pragma once
#include <vector>

#include "AbstractQueueManager.h"

enum class QueueActionType : char;
class QueueElement;

class QueueManager : public AbstractQueueManager {
public:
	explicit QueueManager(unsigned short maxCapacity);
	~QueueManager();

	void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) override;
	QueueElement* update() override;
	short getSize() const override;
	QueueElement* getAt(short i) override;
private:
	std::vector<QueueElement*> queue;

	unsigned short maxCapacity;
};
