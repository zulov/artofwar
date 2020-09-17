#pragma once
class Physical;

namespace Urho3D {
	class String;
}

class InfluenceMapI {
public:
	virtual void update(Physical* thing, float value = 1) =0;
	virtual void reset() =0;
	virtual void finishCalc() =0;
	virtual void print(Urho3D::String name) =0;
	virtual int getMaxElement() =0;
};
