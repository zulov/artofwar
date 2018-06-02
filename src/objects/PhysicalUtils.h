#pragma once
#include "Physical.h"

bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}
