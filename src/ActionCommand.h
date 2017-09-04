#pragma once
#include <vector>
#include "Entity.h"
#include "AbstractCommand.h"
#include "Simulation.h"

class ActionCommand :public AbstractCommand
{
public:
	ActionCommand(std::vector<Physical*>* entities, ActionType action, Vector3* paremater);
	ActionCommand(Physical* entity, ActionType action, Vector3* paremater);
	~ActionCommand();
	void applyAim(ActionParameter* localParameter);
	void execute() override;
	void setAimConteiner(AimContainer* _aimContainer);
private:
	std::vector<Physical*>* entities;
	Physical* entity;
	ActionType action;

	AimContainer* aimContainer;
	Vector3* aimPosition;

	void applyAim(Aims* aims);

};
