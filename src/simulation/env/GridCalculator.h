#pragma once
#include <Urho3D/Math/Vector3.h>

struct GridCalculator {

	explicit GridCalculator(unsigned short resolution, float size)
		: sqResolution(resolution * resolution), resolution(resolution), halfResolution(resolution / 2),
		  halfSize(size * 0.5f), invFieldSize(static_cast<float>(resolution) / size),
		  fieldSize(size / static_cast<float>(resolution)) { }

	GridCalculator(const GridCalculator&) = delete;

	int getIndex(short posX, short posZ) const {
		return getNotSafeIndex(getValid(posX), getValid(posZ));
	}

	int getNotSafeIndex(unsigned short posX, unsigned short posZ) const {
		return posX * resolution + posZ;
	}

	short getNotSafeIndexClose(short posX, short posZ) const {
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

	Urho3D::IntVector2 getShiftCords(int i) const {
		auto cord = getIndexes(i);

		if (cord.y_ <= -halfResolution) {
			if (cord.x_ > 0) {
				++cord.x_;
			} else {
				--cord.x_;
			}
			cord.y_ = i - (cord.x_ * resolution);
			return cord;
		}
		if (cord.y_ >= halfResolution) {
			if (cord.x_ >= 0) {
				++cord.x_;
			} else {
				--cord.x_;
			}
			cord.y_ = i - (cord.x_ * resolution);
			return cord;
		}

		return cord;
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

	float getDistance(const Urho3D::IntVector2& a, int next) const {
		const auto b = getIndexes(next);
		const auto x = (a.x_ - b.x_) * fieldSize;
		const auto y = (a.y_ - b.y_) * fieldSize;
		return sqrt(x * x + y * y);
	}

	float getDistance(int first, int next) const {
		return getDistance(getIndexes(first), next);
	}

	float getSize() const {
		return fieldSize * resolution;
	}

private:
	unsigned int sqResolution;
	unsigned short resolution;
	unsigned short halfResolution;
	float halfSize;
	float invFieldSize;
	float fieldSize;
};
