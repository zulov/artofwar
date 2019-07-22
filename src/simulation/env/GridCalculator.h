#pragma once
#include <Urho3D/Math/Vector3.h>

struct GridCalculator {

	explicit GridCalculator(unsigned short resolution, float size)
		: resolution(resolution), halfRes(resolution / 2), invFieldSize(resolution / size), size(size) {
	}

	int getIndex(short posX, short posZ) const { return posX * resolution + posZ; }

	short getIndex(float value) const {
		if (value < 0) {
			short index = (short)(value * invFieldSize) + halfRes - 1;
			if (index >= 0) {
				return index;
			}
			return 0;
		}
		short index = (short)(value * invFieldSize) + halfRes;
		if (index < resolution) {
			return index;
		}
		return resolution - 1; //TODO czy aby napewno?
	}

	int indexFromPosition(Urho3D::Vector3& pos) const {
		return getIndex(getIndex(pos.x_), getIndex(pos.z_));
	}

	int indexFromPosition(const Urho3D::Vector2& pos) const {
		return getIndex(getIndex(pos.x_), getIndex(pos.y_));
	}

private:
	short resolution;
	short halfRes;
	float invFieldSize;
	float size;
};
