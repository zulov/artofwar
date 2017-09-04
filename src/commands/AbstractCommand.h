#pragma once
class AbstractCommand
{
public:
	AbstractCommand();
	virtual ~AbstractCommand();
	virtual void execute() =0;
};
