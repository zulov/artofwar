#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>

struct GridCalculator {
	explicit GridCalculator(unsigned short resolution, float size)
		: resolution(resolution), halfResolution(resolution / 2), sqResolution(resolution * resolution),
		  fieldSize(size / static_cast<float>(resolution)), halfSize(size * 0.5f),
		  firstCenter(0.5f * fieldSize - halfSize),
		  invFieldSize(static_cast<float>(resolution) / size), sqFieldSize(fieldSize * fieldSize),
		  shiftAmount(log2(resolution)), mask(resolution - 1), lastIndex(resolution - 1) {
		assert(((resolution & (resolution - 1)) == 0));
	}

	GridCalculator(const GridCalculator&) = delete;

	int getIndex(short posX, short posZ) const {
		return getNotSafeIndex(getValid(posX), getValid(posZ));
	}

	int getNotSafeIndex(unsigned short posX, unsigned short posZ) const {
		assert((posX * resolution + posZ) >= 0 && (posX * resolution + posZ) < sqResolution);
		return posX * resolution + posZ;
	}

	short getNotSafeIndexClose(short posX, short posZ) const {
		//TODO better to przyjmuje tylko ujemne, uzyc tego wyzej i odwroci?
		assert(abs(posX)<=34);
		assert((posX * resolution + posZ) > -(int)sqResolution && (posX * resolution + posZ) < (int)sqResolution);
		return posX * resolution + posZ;
	}

	unsigned short getIndex(float value) const {
		return getValid((value + halfSize) * invFieldSize);
	}

	std::pair<unsigned short, unsigned short> getIndex(std::pair<float, float> range) const {
		return {
			getValid((range.first + halfSize) * invFieldSize),
			getValid((range.second + halfSize) * invFieldSize)
		};
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

	Urho3D::IntVector2 getCords(int i) const {
		//return {i / resolution, i % resolution};
		return {i >> shiftAmount, i & mask};
		//Zamiast dzielenia przez "resolution" u¿yto zmiennej "shiftAmount", która jest równe log2(resolution), a zamiast modulo(operator %) u¿yto maski,
		//która ma wartosc "resolution - 1" i ma wype³nione jedynkami tylko najmniej znacz¹ce bity.
	}

	Urho3D::IntVector2 getCords(const Urho3D::Vector2& pos) const {
		return {getIndex(pos.x_), getIndex(pos.y_)};
	}

	Urho3D::Vector2 getCenter(int i) const {
		const auto c = getCords(i);
		return getCenter(c.x_, c.y_);
	}

	Urho3D::Vector2 getCenter(int x, int y) const {
		return {firstCenter + x * fieldSize, firstCenter + y * fieldSize};
	}

	bool isValidIndex(short x, short z) const {
		return !(x < 0 || x >= resolution || z < 0 || z >= resolution);
	}

	bool isValidIndex(const Urho3D::IntVector2& cord) const {
		return !(cord.x_ < 0 || cord.x_ >= resolution || cord.y_ < 0 || cord.y_ >= resolution);
	}

	bool isValidIndex(int index) const { return index >= 0 && index < sqResolution; }

	float getFieldSize() const { return fieldSize; }

	unsigned short getValidLow(short val) const {
		return val >= 0 ? val : 0;
	}

	unsigned short getValidHigh(short val) const {
		return val < resolution ? val : resolution - 1;
	}

	unsigned short getValid(short val) const {
		return std::clamp(val, static_cast<short>(0), lastIndex);
	}

	unsigned short getResolution() const { return resolution; }

	float getSqDistance(const Urho3D::IntVector2& a, int next) const {
		const auto b = getCords(next);

		return getSqDistance(a, b);
	}

	float getSqDistance(const Urho3D::IntVector2& a, const Urho3D::IntVector2& b) const {
		const auto dx = (a.x_ - b.x_);
		const auto dy = (a.y_ - b.y_);
		return (dx * dx + dy * dy) * sqFieldSize;
	}

	int getBiggestManhattan(int first, const std::vector<Urho3D::IntVector2>& endCords) const {
		const auto a = getCords(first);
		int max = 0;
		for (const auto& b : endCords) {
			const auto val = Urho3D::Abs(a.x_ - b.x_) + Urho3D::Abs(a.y_ - b.y_);
			if (val > max) {
				max = val;
			}
		}
		return max;
	}

	float getSize() const {
		return fieldSize * resolution;
	}

private:
	unsigned short resolution;
	unsigned short halfResolution;
	unsigned int sqResolution;
	float fieldSize;
	float halfSize;
	float firstCenter;
	float invFieldSize;
	float sqFieldSize;
	unsigned char shiftAmount;
	unsigned short mask;
	short lastIndex;
};
