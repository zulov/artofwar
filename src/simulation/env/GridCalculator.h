#pragma once
#include <iostream>
#include <Urho3D/Math/Vector3.h>

struct GridCalculator {

	explicit GridCalculator(unsigned short resolution, float size)
		: sqResolution(resolution * resolution), resolution(resolution), halfRes(resolution / 2),
		  invFieldSize(resolution / size), fieldSize(size / resolution), size(size) {
	}

	GridCalculator(const GridCalculator&) = delete;

	int getIndex(short posX, short posZ) const {
		return getNotSafeIndex(getValid(posX), getValid(posZ));
	}

	int getNotSafeIndex(short posX, short posZ) const {
		return posX * resolution + posZ;
	}

	short getIndex2(float value) const {
		return  getValid((short)(value+(size/2.f))*invFieldSize);
	}
	short getIndex(float value) const {
		//auto test = getValid((short)(value+halfRes)*invFieldSize);
		if (value < 0.f) {
			const short index = (short)(value * invFieldSize) + halfRes - 1;
			if (index >= 0) {
				return index;
			}
			return 0;
		}
		const short index = (short)(value * invFieldSize) + halfRes;
		if (index < resolution) {
			return index;
		}
		return resolution - 1;
	}

	int indexFromPosition(const Urho3D::Vector3& pos) const {
		return getNotSafeIndex(getIndex(pos.x_), getIndex(pos.z_));
	}

	int indexFromPosition(const Urho3D::Vector2& pos) const {
		auto a = getNotSafeIndex(getIndex2(pos.x_), getIndex2(pos.y_));
		auto b =  getNotSafeIndex(getIndex(pos.x_), getIndex(pos.y_));
		if(a!=b) {
			std::cout<<a <<" # "<<b<<" "<<a-b<<std::endl;
		}else {

			std::cout<<"|";
		}
		return b;
	}

	Urho3D::IntVector2 getIndexes(int i) const {
		return {i % resolution, i / resolution};
	}

	Urho3D::Vector2 getCenter(int i) const {
		Urho3D::IntVector2 pos = getIndexes(i);
		const float cX = (pos.x_ + 0.5f) * fieldSize - size / 2.f;
		const float cZ = (pos.y_ + 0.5f) * fieldSize - size / 2.f;
		return {cZ, cX};
	}

	bool isValidIndex(short x, short z) const {
		return !(x < 0 || x >= resolution || z < 0 || z >= resolution);
	}

	bool isValidIndex(int index) const { return index >= 0 && index < sqResolution; }

	float getFieldSize() const { return fieldSize; }

	short getValid(short val) const {
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

private:
	unsigned int sqResolution;
	unsigned short resolution;
	unsigned short halfRes;
	float invFieldSize;
	float fieldSize;
	float size;
};
