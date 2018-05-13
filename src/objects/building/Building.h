#pragma once
#include "objects/Static.h"
#include "objects/queue/QueueElement.h"
#include "objects/queue/QueueManager.h"

struct db_building;
struct db_building_level;
struct db_unit;

class Building : public Static
{
public:

	Building(Vector3* _position, int id, int player, int level);
	~Building();

	void populate();
	void upgrade(char level);
	static std::string getColumns();

	QueueElement* updateQueue(float time) const { return queue->update(time); }
	Vector2& getTarget() { return target; } //TODO target to nie to samo co gdzie sie maja pojawiac!
	QueueManager* getQueue() const { return queue; }

	String& toMultiLineString() override;
	void absorbAttack(float attackCoef) override;
	void action(char id, ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	float getMaxHpBarSize() override;
	int getDbID() override;
	int getLevel() override;
private:
	Vector2 target;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	std::vector<db_unit*>* units;
	QueueManager* queue;
};
