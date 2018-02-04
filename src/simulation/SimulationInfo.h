#pragma once

#define SIMULATION_INFO_SIZE 6

class SimulationInfo
{
public:
	SimulationInfo();
	~SimulationInfo();
	bool ifUnitDied();
	bool ifUnitAmountChanged();
	void reset();

	void setUnitDied();
	void setAmountUnitChanged();
	void setBuildingDied();
	void setAmountBuildingChanged();
	void setResourceDied();
	void setAmountResourceChanged();
	void set(SimulationInfo* simulationInfo);
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
