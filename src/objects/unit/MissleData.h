#pragma once
#include "objects/Physical.h"
#include "objects/NodeUtils.h"
#include <Urho3D/Math/Vector3.h>

struct MissleData
{
	Urho3D::Vector3 start;
	Urho3D::Vector3 end;
	Physical* aim;
	float peakHeight;
	float speed;
	Node* node;

	~MissleData() {
		reset();
	}

	MissleData(float peakHeight, float speed)
		: peakHeight(peakHeight),
		speed(speed) {
	}

	void init(Vector3& _start, Vector3& _end, Physical* _aim) {
		start = _start;
		end = _end;
		aim = _aim;
	}

	void createObject(String modelName, String textureName) {
		createNode(textureName, modelName, &node);
	}

	bool update(float timeStep) {

	}


	void reset() {
		if (node) {
			node->Remove();
			node = nullptr;
		}
	}
};
