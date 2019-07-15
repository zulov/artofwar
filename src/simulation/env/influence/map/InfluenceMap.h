#pragma once
class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short resolution);
	virtual ~InfluenceMap();

	virtual void update(Physical* physical) =0;
	virtual void reset() =0;

protected:
	unsigned short resolution;
	unsigned int arraySize;
};
