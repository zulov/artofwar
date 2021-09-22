#pragma once
#include "objects/NodeUtils.h"
#include "math/MathUtils.h"
#include <Urho3D/Math/Vector3.h>

struct MissileData {
	Urho3D::Vector3 start;
	Urho3D::Vector3 end;
	Urho3D::Vector3 direction;

	//Physical* aim;
	Urho3D::Node* node;

	float distance;
	float distanceSoFar;

	float peakHeight;
	float speed;

	~MissileData() {
		node->Remove();
		node = nullptr;
	}

	MissileData(float peakHeight, float speed)
		: peakHeight(peakHeight),
		  speed(speed) {
		node = createNode("Objects/units/additional/missile.xml");
	}

	MissileData(const MissileData&) = delete;

	void init(const Urho3D::Vector3& start, const Urho3D::Vector3& end, float speed = 7) {
		this->end = end;
		this->start = start;
		this->speed = speed;

		direction = end - start;
		distance = direction.Length();
		direction.Normalize();
		direction *= speed;

		peakHeight = distance / 3;

		distanceSoFar = 0;

		node->SetEnabled(true);
		node->SetPosition(start);
	}

	bool update(float timeStep) {
		distanceSoFar += speed * timeStep;
		auto pos = node->GetPosition();
		pos += direction * timeStep; //TODO uwzglednic tylko 2 wymiary
		pos.y_ = sin(distanceSoFar * Urho3D::M_PI / distance) * peakHeight + start.y_;
		node->SetDirection(pos - node->GetPosition());
		node->SetPosition(pos);

		if (finished()) {
			reset();
			return true;
			// if (aim && aim->isAlive()
			// 	&& sqDist(aim->getPosition(), getPosition()) < 3 * 3) {
			// 	return aim->absorbAttack(attackCoef);
			// }
			//
		}
		return false;
	}

	void reset() const {
		node->SetEnabled(false);
	}

	bool isUp() const {
		return node->IsEnabled();
	}

	bool finished() const {
		return distanceSoFar >= distance && node->IsEnabled();
	}

	const Urho3D::Vector3& getPosition() const {
		return node->GetPosition();
	}
};
