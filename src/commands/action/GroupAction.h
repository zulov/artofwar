#pragma once
#include "ActionCommand.h"

class GroupAction : public ActionCommand
{
public:
	GroupAction(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append = false);
	GroupAction(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append = false);
	~GroupAction();
private:
	std::vector<Physical*>* entities;
};
