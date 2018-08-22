#pragma once
#include "objects/static/Static.h"
#include "objects/queue/QueueManager.h"

struct db_building;
struct db_building_level;
struct db_unit;
class QueueElement;

class Building : public Static
{
public:
	Building(Urho3D::Vector3* _position, int id, int player, int level, int mainCell);
	~Building();

	void populate();
	void upgrade(char level);
	static std::string getColumns();

	QueueElement* updateQueue(float time) const { return queue->update(time); }
	Urho3D::Vector2& getTarget() { return target; } //TODO target to nie to samo co gdzie sie maja pojawiac!
	QueueManager* getQueue() const { return queue; }

	Urho3D::String& toMultiLineString() override;
	void absorbAttack(float attackCoef) override;
	void action(char id, const ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	float getMaxHpBarSize() override;
	int getDbID() override;
	int getLevel() override;
private:
	Urho3D::Vector2 target;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	std::vector<db_unit*>* units;
	QueueManager* queue;
};
