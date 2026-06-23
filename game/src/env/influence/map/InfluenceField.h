#pragma once

#include <array>
#include <optional>
#include <span>
#include <vector>

#include <Urho3D/Math/Vector2.h>

#include "InfluenceMap.h"

class InfluenceField : public InfluenceMap {
public:
	InfluenceField(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
	                  float* sharedTemplateV, bool kernelView = true, bool ownsTemplateV = false);
	InfluenceField(unsigned short resolution, float size, float valueThresholdDebug);
	InfluenceField(unsigned short resolution, float size);
	~InfluenceField() override;

	void update(unsigned index, float value = 1.f);
	void update(const Urho3D::Vector2& pos, float value = 1.f);
	void updateInt(unsigned index, unsigned char value = 1);
	void updateFromTemp();

	void reset() override;

	float getRaw(unsigned index) const;
	float getRaw(const Urho3D::Vector2& pos) const;
	float getKernel(unsigned index) const;
	float getKernel(const Urho3D::Vector2& pos) const;
	float getKernal(unsigned index) const { return getKernel(index); }
	float getKernal(const Urho3D::Vector2& pos) const { return getKernel(pos); }

	float getValueAt(unsigned index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	std::optional<Urho3D::Vector2> getCenter() const;
	std::vector<unsigned> getMaxIdxs() const;

	std::vector<int> getIndexesWithByValue(float percent, float tolerance);
	bool cumulateErrors(float percent, float* intersection);

	void ensureReady() override;

	static float* createTemplateV(float coef, char level);

protected:
	float* rawValues;
	float* values;
	float coef;
	mutable bool valuesCalculateNeeded = false;
	void invalidateCaches();

private:
	enum class DefaultView { RAW, KERNEL };

	const float* viewData() const;
	void ensureKernel() const;
	void ensureQuad() const;
	void rebuildKernel() const;
	void rebuildQuad() const;
	int getMaxElement(const std::array<int, 4>& indexes, std::span<float> vals) const;
	void computeMinMax() const override;
	unsigned char level;
	unsigned char levelRes;
	float* templateV;
	bool ownsTemplateV = false;
	float* quadValues;
	unsigned int quadArraySize;
	std::vector<std::span<float>> quadLayers;
	std::vector<unsigned short> quadResolutions;
	DefaultView defaultView;
	mutable bool quadDirty = true;
};
