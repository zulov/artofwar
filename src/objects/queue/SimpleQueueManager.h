#pragma once
#include "AbstractQueueManager.h"
class QueueElement;

class SimpleQueueManager : public AbstractQueueManager {
public:
	SimpleQueueManager() = default;
	~SimpleQueueManager();
	void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) override;
	QueueElement* update(float time) override;
	short getSize() const override;
	QueueElement* getAt(short i) override;
private:
	QueueElement* element = nullptr;
};
