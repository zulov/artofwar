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
		node->Remove();
		node = nullptr;
	}

	MissleData(float peakHeight, float speed)
		: peakHeight(peakHeight),
		speed(speed) {
		createNode("Models/Cube.mdl", "Materials/grey.xml", &node);
		node->SetEnabled(false);
	}

	void init(Vector3& _start, Vector3& _end, Physical* _aim) {
		start = _start;
		end = _end;
		aim = _aim;
		node->SetEnabled(true);
		node->SetPosition(start);
	}

	void createObject(String modelName, String textureName) {
		createNode(textureName, modelName, &node);
	}

	bool update(float timeStep) {
		auto pos = node->GetPosition();
		pos.y_ += timeStep;
		node->SetPosition(pos);
		return false;
	}


	void reset() const {
		node->SetEnabled(false);
	}

	bool isUp() const {
		return node->IsEnabled();
	}

	bool finished() {
		return false;
	}
};
