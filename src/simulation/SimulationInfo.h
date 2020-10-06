#pragma once

enum class ObjectType : char;

class SimulationInfo {
public:
	SimulationInfo();
	~SimulationInfo() = default;

	void reset();

	void setAmountUnitChanged(); 
	void setAmountBuildingChanged();
	void setAmountResourceChanged();

	void setSthDied(ObjectType type);

	void set(SimulationInfo& simulationInfo);

	bool ifUnitDied() const { return unitDied; }
	bool ifAmountUnitChanged() const { return amountUnitChanged; }
	bool ifBuildingDied() const { return buildingDied; }
	bool ifAmountBuildingChanged() const { return amountBuildingChanged; }
	bool ifResourceDied() const { return resourceDied; }
	bool ifAmountResourceChanged() const { return amountResourceChanged; }

	void setCurrentFrame(unsigned char currentFrameNumber);
	int getCurrentFrame() const { return currentFrameNumber; }
private:
	void setUnitDied();
	void setBuildingDied();
	void setResourceDied();

	bool unitDied;
	bool amountUnitChanged;

	bool buildingDied;
	bool amountBuildingChanged;

	bool resourceDied;
	bool amountResourceChanged;

	unsigned char currentFrameNumber;

};
