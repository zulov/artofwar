#pragma once
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

struct db_container
{
	db_unit unit;
};
