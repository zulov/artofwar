#pragma once
struct GridCalculator {

	explicit GridCalculator(short resolution)
		: resolution(resolution), halfRes(resolution / 2) {}

	int getIndex(short posX, short posZ) const { return posX * resolution + posZ; }

	short getIndex(float value) const {
		if (value < 0) {
			short index = (short)(value * invFieldSize) + halfResolution - 1;
			if (index >= 0) {
				return index;
			}
			return 0;
		}
		short index = (short)(value * invFieldSize) + halfResolution;
		if (index < resolution) {
			return index;
		}
		return resolution - 1; //TODO czy aby napewno?
	}

	int indexFromPosition(Urho3D::Vector3& pos) const {
		return getIndex(getIndex(pos.x_), getIndex(pos.z_));
	}

	int indexFromPosition(Urho3D::Vector2& pos) const {
		return getIndex(getIndex(pos.x_), getIndex(pos.y_));
	}

private:
	short resolution;
	short halfRes;
};
