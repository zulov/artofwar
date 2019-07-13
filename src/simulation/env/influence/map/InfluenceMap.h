#pragma once
class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short size);
	virtual ~InfluenceMap();

	virtual void update(Physical* physical) =0;
	virtual void reset() =0;

protected:
	unsigned short size;
};
