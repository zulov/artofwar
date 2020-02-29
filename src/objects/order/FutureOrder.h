#pragma once

enum class UnitActionType : char;

class FutureOrder {
public:
	FutureOrder(UnitActionType actionType, short id, bool append): actionType(actionType), append(append), id(id) {
	}

	virtual ~FutureOrder() = default;

	virtual bool expired() =0;
	virtual bool add() =0;
	virtual void clean() =0;
	virtual void execute() =0;

	bool getAppend() const { return append; }
	UnitActionType getAction() const { return actionType; }
	short getId() const { return id; }
protected:
	const UnitActionType actionType;
	const bool append;
	const short id;
};
