#pragma once
#include <vector>

class Layer {

public:
	Layer(std::vector<float>& w, std::vector<float>& bias);
	~Layer();
	void setValues(float data[]) const;
	float* getValues() const { return values; }
	short getNumberOfValues() const { return numberOfNodes; }
	short getPrevSize() const { return prevSize; }
	float* getW() const { return w; }
	short getWSize() const { return numberOfNodes * prevSize; }
	float getBias(int i) const { return bias[i]; }
	void setValueAt(int i, float newValue) const { values[i] = newValue; }
private:

	short numberOfNodes;
	short prevSize;

	float* values;
	float* bias;

	float* w;

};
