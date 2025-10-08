#pragma once

enum class QueueActionType : char;
class QueueElement;

class QueueManager  {
public:
	explicit QueueManager(unsigned short maxCapacity);
	~QueueManager();

	void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity);
	QueueElement* update();
	short getSize() const;
	QueueElement* getAt(short i);
	QueueElement* first();
	void resize(short maxSize);

private:
	QueueElement** queue{};

	unsigned short maxCapacity;
};
