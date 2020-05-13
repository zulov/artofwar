#pragma once
#include "objects/Physical.h"
#include "objects/NodeUtils.h"
#include "math/MathUtils.h"
#include <Urho3D/Math/Vector3.h>

struct MissileData {
	Urho3D::Vector3 start;
	Urho3D::Vector3 end;
	Urho3D::Vector3 direction;

	Physical* aim;
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
		createNode("Models/units/additional/Prism.mdl", "Materials/units/additional/brown.xml", &node);
		node->SetEnabled(false);
		node->SetScale(Urho3D::Vector3(0.1, 0.1, 0.3));
	}

	MissileData(const MissileData&) = delete;

	void init(Urho3D::Vector3& _start, Urho3D::Vector3& _end, Physical* _aim, float _speed = 7) {
		start = _start;
		end = _end;
		aim = _aim;
		speed = _speed;

		direction = end - start;
		distance = direction.Length();
		direction.Normalize();
		direction *= speed;

		peakHeight = distance / 3;

		distanceSoFar = 0;

		node->SetEnabled(true);
		node->SetPosition(start);
	}

	void update(float timeStep, float attackCoef) {
		distanceSoFar += speed * timeStep;
		auto pos = node->GetPosition();
		pos += direction * timeStep; //TODO uwzglednic tylko 2 wymiary
		pos.y_ = sin(distanceSoFar * Urho3D::M_PI / distance) * peakHeight + start.y_;
		node->SetDirection(pos - node->GetPosition());
		node->SetPosition(pos);

		if (finished()) {
			if (aim && aim->isAlive()
				&& sqDist(aim->getPosition(), getPosition()) < 3 * 3) {
				aim->absorbAttack(attackCoef);
			}
			reset();
		}

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
