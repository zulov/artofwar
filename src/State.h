#pragma once
class State
{
public:
	State();
	~State();
	virtual void onStart() =0;
	virtual void onEnd() =0;
};
