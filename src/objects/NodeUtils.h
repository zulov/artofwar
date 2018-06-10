#pragma once
#include "Game.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>

inline void createNode(const Urho3D::String& model, const Urho3D::String& texture, Urho3D::Node** node) {
	*node = Game::getScene()->CreateChild();
	auto selectionModel = (*node)->CreateComponent<Urho3D::StaticModel>();
	selectionModel->SetModel(Game::getCache()->GetResource<Urho3D::Model>(model));
	selectionModel->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(texture));
	(*node)->SetEnabled(false);
}
