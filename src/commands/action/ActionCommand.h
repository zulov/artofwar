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
	ActionParameter getTargetAim(Physical * physical);
	ActionParameter getFollowAim(Physical* toFollow);
	ActionParameter getChargeAim(Vector3* charge);
	void createTargetAim(int bucketInx, ActionParameter& parameter);

	void execute() override;
	void setAimConteiner(AimContainer* _aimContainer);
private:
	void addTargetAim();
	void addFollowAim(Physical* toFollow);
	void addChargeAim(Vector3* charge);
	void appendAim();
	
	std::vector<Physical*>* entities;
	Physical* entity;

	OrderType action;
	Vector3* vector;
	Physical* toFollow;

	AimContainer* aimContainer;


};
