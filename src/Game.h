#pragma once
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

class Game
{
public:
	static Game* getInstance();
	~Game();
	void setCache(Urho3D::ResourceCache* _cache);
	void setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene);

	Urho3D::ResourceCache* getCache() const;
	Urho3D::SharedPtr<Urho3D::Scene> getScene() const;
private:
	Game();
	static Game* instance;
	Urho3D::ResourceCache* cache;
	Urho3D::SharedPtr<Urho3D::Scene> scene;

};

