#pragma once
#include <Urho3D/Math/Vector3.h>

struct GridCalculator {

	explicit GridCalculator(unsigned short resolution, float size)
		: sqResolution(resolution * resolution), resolution(resolution),
		  invFieldSize(resolution / size), fieldSize(size / resolution), size(size), halfSize(size / 2.f) { }

	GridCalculator(const GridCalculator&) = delete;

	int getIndex(short posX, short posZ) const {
		return getNotSafeIndex(getValid(posX), getValid(posZ));
	}

	int getNotSafeIndex(unsigned short posX, unsigned short posZ) const {
		return posX * resolution + posZ;
	}

	unsigned short getIndex(float value) const {
		return getValid((short)(value + halfSize) * invFieldSize);
	}

	int indexFromPosition(const Urho3D::Vector3& pos) const {
		return getNotSafeIndex(getIndex(pos.x_), getIndex(pos.z_));
	}

	int indexFromPosition(const Urho3D::Vector2& pos) const {
		return getNotSafeIndex(getIndex(pos.x_), getIndex(pos.y_));
	}

	int indexFromPosition(float x, float z) const {
		return getNotSafeIndex(getIndex(x), getIndex(z));
	}

	Urho3D::IntVector2 getIndexes(int i) const {
		return {i / resolution, i % resolution};
	}

	Urho3D::Vector2 getCenter(int i) const {
		const Urho3D::IntVector2 pos = getIndexes(i);
		const float cX = (pos.x_ + 0.5f) * fieldSize - halfSize;
		const float cZ = (pos.y_ + 0.5f) * fieldSize - halfSize;
		return {cX, cZ};
	}

	bool isValidIndex(short x, short z) const {
		return !(x < 0 || x >= resolution || z < 0 || z >= resolution);
	}

	bool isValidIndex(int index) const { return index >= 0 && index < sqResolution; }

	float getFieldSize() const { return fieldSize; }

	unsigned short getValid(short val) const {
		if (val < 0) {
			return 0;
		}
		if (val >= resolution) {
			return resolution - 1;
		}
		return val;
	}

	unsigned short getResolution() const { return resolution; }
	float getSize() const { return size; }

	float getDistance(const Urho3D::IntVector2& a, int next) const {
		const auto b = getIndexes(next);
		const auto x = (a.x_ - b.x_) * fieldSize;
		const auto y = (a.y_ - b.y_) * fieldSize;
		return sqrt(x * x + y * y);
	}

	float getDistance(int first, int next) const {
		return getDistance(getIndexes(first), next);
	}


private:
	unsigned int sqResolution;
	unsigned short resolution;
	float halfSize;
	float invFieldSize;
	float fieldSize;
	float size;
};
