#pragma once
#define UNITS_NUMBER_DB 50
#define BULDINGS_NUMBER_DB 50
#include <Urho3D/Container/Str.h>

struct db_unit
{
	Urho3D::String name;
	double minDist;
	double maxSep;
	int type;
	Urho3D::String model;
	Urho3D::String texture;
	Urho3D::String font;
	double mass;
	double maxSpeed;
	double scale;

	db_unit(char* name, double minDist, double maxSep, int type, char* model, char* texture, char* font, double mass, double maxSpeed, double scale)
		: name(name),
		minDist(minDist),
		maxSep(maxSep),
		type(type),
		model(model),
		texture(texture),
		font(font),
		mass(mass),
		maxSpeed(maxSpeed),
		scale(scale) {
	}

	db_unit() {
	};
};

struct db_building
{
	Urho3D::String name;
	double minDist;
	int type;
	Urho3D::String model;
	Urho3D::String texture;
	Urho3D::String font;
	double scale;

	db_building() {
	};
};

struct db_container
{
	db_unit* units[UNITS_NUMBER_DB];
	db_building* buildings[BULDINGS_NUMBER_DB];
};
