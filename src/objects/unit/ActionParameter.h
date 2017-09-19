#pragma once

#include "aim/Aims.h"

class Physical;

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

