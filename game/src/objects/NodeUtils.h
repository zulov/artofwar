#pragma once
#include "Game.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>

#include "Physical.h"


inline Urho3D::Node* createNode(const Urho3D::String& name) {
	const auto node = Game::getScene()->CreateChild();
	node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(name)->GetRoot());
	return node;
}

inline void changeMaterial(Urho3D::Material* newMaterial, Urho3D::StaticModel* model) {
	if (model && newMaterial && newMaterial != model->GetMaterial(0)) {
		model->SetMaterial(newMaterial);
	}
}

inline void changeMaterial(const Urho3D::String& name, Urho3D::StaticModel* model) {
	changeMaterial(Game::getCache()->GetResource<Urho3D::Material>(name), model);
}

inline void setShaderParam(Urho3D::Node * node, const Urho3D::String& name, const Urho3D::Variant& value) {
	if (node) {
		const auto comp = node->GetComponent<Urho3D::StaticModel>();

		if (comp) {
			for (int i = 0; i < comp->GetNumGeometries(); ++i) {
				comp->GetMaterial(i)->SetShaderParameter(name, value);
			}
		}
	}
}

inline void setShaderParam(Physical* phy, const Urho3D::String& name, const Urho3D::Variant& value) {
	phy->ensureMaterialCloned();
	setShaderParam(phy->getNode(), name, value);
}
