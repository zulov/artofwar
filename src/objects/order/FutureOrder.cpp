#include "FutureOrder.h"

FutureOrder::FutureOrder(char actionType, short id, bool append): action(actionType), id(id), append(append) {
}

FutureOrder::~FutureOrder() = default;
