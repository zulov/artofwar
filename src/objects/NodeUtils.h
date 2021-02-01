#pragma once
#include "Game.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

inline void createNode(const Urho3D::String& model, const Urho3D::String& texture, Urho3D::Node** node, bool enabled = false) {
	*node = Game::getScene()->CreateChild();
	auto selectionModel = (*node)->CreateComponent<Urho3D::StaticModel>();
	selectionModel->SetModel(Game::getCache()->GetResource<Urho3D::Model>(model));
	selectionModel->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(texture));
	(*node)->SetEnabled(enabled);
}

inline void changeMaterial(Urho3D::Material* newMaterial, Urho3D::StaticModel* model) {
	if (newMaterial && newMaterial != model->GetMaterial(0)) {
		model->SetMaterial(newMaterial);
	}
}

inline void showNode(Urho3D::Node* node, Urho3D::Vector3& pos) {
	node->SetEnabled(true);
	node->SetScale(1);
	node->SetPosition(pos);
}