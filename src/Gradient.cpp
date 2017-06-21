#include "Gradient.h"
#include <iostream>


Gradient::Gradient(int _resolution, double _size) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	values = new Urho3D::Vector3**[resolution];

	for (int i = 0; i < resolution; ++i) {
		values[i] = new Urho3D::Vector3 *[resolution];
	}
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			values[i][j] = new Urho3D::Vector3();
		}
	}
}

Gradient::~Gradient() {
}

Urho3D::Vector3* Gradient::getValueAt(double x, double z) {//TODO zrobic srednie z sasiednich
	return values[getIntegerPos(x) + halfResolution][getIntegerPos(z) + halfResolution];
}

int Gradient::getIntegerPos(double value) {
	return getDoublePos(value);
}

double Gradient::getDoublePos(double value) {
	if (value < 0) {
		return (value / size * resolution);
	}
	return (value / size * resolution);
}

void Gradient::add(Entity* entity) {
	Urho3D::Vector3* pos = entity->getPosition();

	int dX = getIntegerPos(pos->x_) + halfResolution;
	int dZ = getIntegerPos(pos->z_) + halfResolution;

	double ddX = getDoublePos(pos->x_) + halfResolution;
	double ddZ = getDoublePos(pos->z_) + halfResolution;
	double bucketSize = (size / resolution);
	double level = entity->getMinimalDistance() / (bucketSize) * 2;
	Vector3 position(ddX, 0, ddZ);
	for (int i = -level; i <= level; ++i) {//TODO zwiekszyc zasieg
		for (int j = -level; j <= level; ++j) {

			double centerX = dX + (i + 0.5) * bucketSize;
			double centerZ = dZ + (j + 0.5) * bucketSize;

			Vector3 center(centerX, 0, centerZ);
			Vector3 dir = center - position;
			double distance = dir.Length();
			dir /= distance;
			double minimalDistance = entity->getMinimalDistance();
			double coef = calculateCoef(distance, minimalDistance);
			dir *= coef;
			(*values[dX + i][dZ + j]) += dir;
		}
	}
}

double Gradient::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist/2;
	if (parameter <= 0.05) {
		parameter = 0.05;
	}
	double coef = exp(1 / parameter) - 1;
	return exp(minDist / (distance + 0.05)) - 1+ coef;

}
