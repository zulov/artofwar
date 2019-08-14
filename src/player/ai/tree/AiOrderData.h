#pragma once
enum class AiOrderType : char;

struct AiOrderData {

	AiOrderData(AiOrderType type, char id)
		: type(type), id(id) {
	}

	const AiOrderType type;
	const char id;
};
