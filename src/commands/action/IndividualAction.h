#pragma once
#include "ActionCommand.h"

class IndividualAction : public ActionCommand
{
public:
	IndividualAction(Physical* entity, OrderType action, Physical* paremeter, bool append = false);
	~IndividualAction();
private:
	Physical* entity;
};
