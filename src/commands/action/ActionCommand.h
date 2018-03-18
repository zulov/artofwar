#pragma once
#include <vector>

#include "OrderType.h"
#include "commands/AbstractCommand.h"
#include "objects/Physical.h"


class ActionCommand : public AbstractCommand
{
public:
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append = false);
	//TODO ten vector trzeba skopiowac raczej
	ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append = false);
	ActionCommand(Physical* entity, OrderType action, Physical* paremeter, bool append = false);
	~ActionCommand();
	ActionParameter getTargetAim(int startIdx, Vector2& to, bool append);
	ActionParameter getFollowAim(Physical* toFollow, bool append);
	ActionParameter getChargeAim(Vector2* charge, bool append);
	void calculateCenter(Vector2& center);

	void execute() override;
private:
	void addTargetAim(Vector2* to, bool append);
	void addFollowAim(Physical* toFollow, bool append);
	void addChargeAim(Vector2* charge, bool append);
	void appendAim();

	std::vector<Physical*>* entities;
	Physical* entity;

	OrderType action;
	Vector2* vector;
	Physical* toFollow;

	bool append;
};
