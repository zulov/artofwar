#pragma once
#include <src/Core/DenseStorage.h>
#include <Core>

class Layer {

public:
	Layer(short numberOfNodes, short prevSize);
	~Layer();
	void setValues(double data[]);
	double* getValues() const { return values; }
	short getNumberOfValues() const { return numberOfNodes; }
	short getPrevSize() const { return prevSize; }
	double * getW() const { return w; }
	short getWSize() const { return numberOfNodes * prevSize; }
	double getBias(int i) const { return bias[i]; }
	void setValueAt(int i, double newValue) { values[i] = newValue; }
private:

	short numberOfNodes;
	short prevSize;

	double* values;
	double* bias;

	double* w;

};
