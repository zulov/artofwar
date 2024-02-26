#pragma once
class PhysicalCommand {
public:
	virtual ~PhysicalCommand() = default;
	virtual void execute() =0;
};
