#pragma once

#define SIMULATION_INFO_SIZE 6
#include "ObjectEnums.h"

class SimulationInfo
{
public:
	SimulationInfo();
	~SimulationInfo();
	bool ifUnitDied();
	bool ifUnitAmountChanged() const;
	void reset() const;

	void setUnitDied() const;
	void setAmountUnitChanged() const;
	void setBuildingDied() const;
	void setAmountBuildingChanged() const;
	void setResourceDied() const;
	void setAmountResourceChanged() const;

	void setSthDied(ObjectType type) const;

	void set(SimulationInfo& simulationInfo);
	void setUnitsNumber(int _untisNumber);

	bool ifUnitDied() const;
	bool ifAmountUnitChanged() const;
	bool ifBuildingDied() const;
	bool ifAmountBuildingChanged() const;
	bool ifResourceDied() const;
	bool ifAmountResourceChanged() const;

	int getUnitsNumber();
private:
	bool flags[SIMULATION_INFO_SIZE];

	bool* unitDied;
	bool* amountUnitChanged;

	bool* buildingDied;
	bool* amountBuildingChanged;

	bool* resourceDied;
	bool* amountResourceChanged;

	int unitsNumber;
};
