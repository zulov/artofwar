#pragma once

class FutureOrder {
public:
	FutureOrder(char actionType, short id, bool append);

	virtual ~FutureOrder();

	virtual bool expired() =0;
	virtual bool add() =0;
	virtual void clean() =0;
	virtual void execute() =0;

	bool getAppend() const { return append; }
	short getAction() const { return action; }
protected:
	const char action;
	const short id;
	const bool append;
};
