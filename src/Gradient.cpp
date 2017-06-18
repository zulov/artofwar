#include "Gradient.h"


Gradient::Gradient(int _resolution, double _size) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	values = new Urho3D::Vector3**[resolution];

	for (int i = 0; i < resolution; i++) {
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
