#pragma once

#include "AiOrderType.h"

enum class AiOrderType : char;

struct AiOrderData {

	AiOrderData(AiOrderType type, const short id)
		: type(type), id(id) {
	}

	const AiOrderType type;
	const short id;
};
