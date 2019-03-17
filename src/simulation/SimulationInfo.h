#pragma once

#define SIMULATION_INFO_SIZE 6

enum class ObjectType : char;

class SimulationInfo
{
public:
	SimulationInfo();
	~SimulationInfo();

	void reset() const;

	void setAmountUnitChanged() const;
	void setAmountBuildingChanged() const;
	void setAmountResourceChanged() const;

	void setSthDied(ObjectType type) const;

	void set(SimulationInfo& simulationInfo);
	void setUnitsNumber(int _untisNumber);

	bool ifUnitDied() const { return *unitDied; }
	bool ifAmountUnitChanged() const { return *amountUnitChanged; }
	bool ifBuildingDied() const { return *buildingDied; }
	bool ifAmountBuildingChanged() const { return *amountBuildingChanged; }
	bool ifResourceDied() const { return *resourceDied; }
	bool ifAmountResourceChanged() const { return *amountResourceChanged; }

	int getUnitsNumber() const;
	void setCurrentFrame(int currentFrameNumber);
	int getCurrentFrame() const { return currentFrameNumber; }
private:
	void setUnitDied() const;
	void setBuildingDied() const;
	void setResourceDied() const;

	bool flags[SIMULATION_INFO_SIZE];

	bool* unitDied;
	bool* amountUnitChanged;

	bool* buildingDied;
	bool* amountBuildingChanged;

	bool* resourceDied;
	bool* amountResourceChanged;

	int unitsNumber;

	int currentFrameNumber;
};
