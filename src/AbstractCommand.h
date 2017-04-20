#pragma once
class AbstractCommand
{
public:
	AbstractCommand();
	~AbstractCommand();
	virtual void execute() =0;
};
