#pragma once
#include <vector>

#include "OrderType.h"
#include "commands/AbstractCommand.h"
#include "objects/Physical.h"

class AimContainer;

class ActionCommand :public AbstractCommand
{
public:
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* parameter);//TODO ten vector trzeba skopiowac raczej
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremeter);
	ActionCommand(Physical* entity, OrderType action, Physical* paremeter);
	~ActionCommand();
	
	void execute() override;
	void setAimConteiner(AimContainer* _aimContainer);
private:
	void applyAim();
	void appendAim();
	
	std::vector<Physical*>* entities;
	Physical* entity;

	OrderType action;
	Vector3* vector;
	Physical* toFollow;

	AimContainer* aimContainer;


};
