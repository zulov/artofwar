#pragma once

struct db_config
{
	int precision;
	int map;
};

struct db_load
{
	db_load() {
		config = new db_config();
	}

	~db_load() {
		delete config;
	}

	db_config* config;
};
