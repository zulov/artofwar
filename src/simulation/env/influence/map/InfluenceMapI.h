#pragma once
#include <array>
#include <Urho3D/Math/Vector2.h>
class Physical;

namespace Urho3D {
	class String;
}

class InfluenceMapI {
public:
	virtual void update(Physical* thing, float value = 1.f) =0;
	virtual void updateInt(Physical* thing, int value = 1) =0;
	virtual void reset() =0;

	virtual void print(Urho3D::String name) =0;
	virtual int getMaxElement() =0;
	virtual int getMaxElement(const std::array<int, 4>& indexes) =0;
	virtual unsigned short getResolution() =0;
	virtual Urho3D::Vector2 getCenter(int index) =0;
};
