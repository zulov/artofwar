#pragma once
#include "Aims.h"

class ActionParameter
{
public:
	ActionParameter();
	~ActionParameter();
	Aims* getAims();
	void setAims(Aims* _aims);
private:
	
	Aims * aims;
};

