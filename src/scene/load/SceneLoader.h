#pragma once
#include <Urho3D/Container/Str.h>
#include <sqlite3/sqlite3.h>

class loading;

class SceneLoader
{
public:
	SceneLoader();
	~SceneLoader();
	void createLoad(Urho3D::String fileName);
private:
	int precision;
	loading * loadingState;
	sqlite3* database;
};

