#include "Gradient.h"
#include <iostream>


Gradient::Gradient(short _resolution, double _size) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	values = new Urho3D::Vector2[resolution * resolution];
}

Gradient::~Gradient() {
	delete[] values;
}

Urho3D::Vector2& Gradient::getValueAt(double x, double z) {
	//TODO zrobic srednie z sasiednich
	return values[(getIntegerPos(x) + halfResolution) * resolution + getIntegerPos(z) + halfResolution];
}

int Gradient::getIntegerPos(double value) {
	return getDoublePos(value);
}

double Gradient::getDoublePos(double value) {
	if (value < 0) {
		return (value / size * resolution);//TODO tu chyba jest jaksi b³ad
	}
	return (value / size * resolution);
}

void Gradient::add(Physical* entity) {
	Urho3D::Vector3* pos = entity->getPosition();

	int dX = getIntegerPos(pos->x_) + halfResolution;
	int dZ = getIntegerPos(pos->z_) + halfResolution;

	double ddX = getDoublePos(pos->x_) + halfResolution;
	double ddZ = getDoublePos(pos->z_) + halfResolution;
	double bucketSize = (size / resolution);
	double level = entity->getMinimalDistance() / (bucketSize) * 2;
	Vector2 position(ddX, ddZ);
	for (int i = -level; i <= level; ++i) {
		//TODO zwiekszyc zasieg
		for (int j = -level; j <= level; ++j) {

			double centerX = dX + (i + 0.5) * bucketSize;
			double centerZ = dZ + (j + 0.5) * bucketSize;

			Vector2 center(centerX, centerZ);
			Vector2 dir = center - position;
			double distance = dir.Length();
			dir /= distance;
			double minimalDistance = entity->getMinimalDistance();
			double coef = calculateCoef(distance, minimalDistance);
			dir *= coef;
			values[(dX + i) * resolution + dZ + j] += dir;
		}
	}
}

double Gradient::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist / 2;
	if (parameter <= 0.05) {
		parameter = 0.05;
	}
	return exp(minDist / (distance + 0.05)) + exp(1 / parameter) - 2;

}
