#pragma once

enum class LeftMenuMode : unsigned char {
	BUILDING=0,
	UNIT,
	ORDER,
	RESOURCE
};

enum class LeftMenuSubMode : unsigned char {
	BASIC=0,
	LEVEL,
	UPGRADE
};
