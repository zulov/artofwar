#pragma once
#include <span>
#include <vector>

class Layer {
public:
	Layer(std::vector<float>& w, std::vector<float>& bias);
	Layer(const Layer& rhs) = delete;
	~Layer();
	void setValues(std::span<float> data) const;
	std::span<float> getValues() const  { return valuesSpan; }

	short getNumberOfValues() const { return valuesSpan.size(); }
	short getPrevSize() const { return prevSize; }
	float* getW() const { return w; }
	short getWSize() const { return valuesSpan.size() * prevSize; }
	float getBias(int i) const { return bias[i]; }
	void setValueAt(int i, float newValue) const { values[i] = newValue; }
private:

	std::span<float> valuesSpan;
	short prevSize;

	float* values;//TODO performance replate with eigen Vector
	float* bias;

	float* w;

};
