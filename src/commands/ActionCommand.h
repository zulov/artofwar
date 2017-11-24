#pragma once
#include <vector>
#include "AbstractCommand.h"
#include "simulation/Simulation.h"
#include "OrderType.h"

class ActionCommand :public AbstractCommand
{
public:
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* paremater);//TODO ten vector trzeba skopiowac raczej
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremater);
	ActionCommand(Physical* entity, OrderType action, Vector3* paremater);
	ActionCommand(Physical* entity, OrderType action, Physical* paremater);
	~ActionCommand();
	
	void execute() override;
	void setAimConteiner(AimContainer* _aimContainer);
private:
	void applyAction(ActionParameter* parameter);
	std::vector<Physical*>* entities;
	Physical* entity;
	OrderType action;

	AimContainer* aimContainer;
	Vector3* vector;

	Physical* toFollow;

	void applyAim(Aims* aims);

};
