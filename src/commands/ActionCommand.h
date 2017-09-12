#pragma once
#include <vector>
#include "objects/Entity.h"
#include "AbstractCommand.h"
#include "simulation/Simulation.h"
#include "OrderType.h"

class ActionCommand :public AbstractCommand
{
public:
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* paremater);
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremater);
	ActionCommand(Physical* entity, OrderType action, Vector3* paremater);
	ActionCommand(Physical* entity, OrderType action, Physical* paremater);
	~ActionCommand();
	void applyAim(ActionParameter* localParameter);
	void execute() override;
	void setAimConteiner(AimContainer* _aimContainer);
private:
	std::vector<Physical*>* entities;
	Physical* entity;
	OrderType action;

	AimContainer* aimContainer;
	Vector3* aimPosition;
	Physical* toFollow;

	void applyAim(Aims* aims);

};
