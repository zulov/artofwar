#pragma once

enum class ObjectType : char;

class ObjectsInfo {
public:
	ObjectsInfo();
	~ObjectsInfo() = default;

	void reset();

	void setAmountUnitChanged(); 
	void setAmountBuildingChanged();
	void setAmountResourceChanged();

	bool ifUnitDied() const { return unitDied; }
	bool ifAmountUnitChanged() const { return amountUnitChanged; }
	bool ifBuildingDied() const { return buildingDied; }
	bool ifAmountBuildingChanged() const { return amountBuildingChanged; }
	bool ifResourceDied() const { return resourceDied; }
	bool ifAmountResourceChanged() const { return amountResourceChanged; }

	void setUnitDied();
	void setBuildingDied();
	void setResourceDied();
private:

	bool unitDied;
	bool amountUnitChanged;

	bool buildingDied;
	bool amountBuildingChanged;

	bool resourceDied;
	bool amountResourceChanged;

};