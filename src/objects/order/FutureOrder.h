#pragma once

enum class UnitActionType : char;

class FutureOrder {
public:
	FutureOrder(UnitActionType actionType, short id, bool append): action(actionType), id(id), append(append) {
	}

	virtual ~FutureOrder() = default;

	virtual bool expired() =0;
	virtual bool add() =0;
	virtual void clean() =0;
	virtual void execute() =0;

	bool getAppend() const { return append; }
	UnitActionType getAction() const { return action; }
	short getId() const { return id; }
protected:
	const UnitActionType action;
	const short id;
	const bool append;
};
