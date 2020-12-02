#include "SimInfo.h"

#include "FrameInfo.h"

SimInfo::SimInfo() {
	frameInfo = new FrameInfo();
}

SimInfo::~SimInfo() { delete frameInfo; }

void SimInfo::setIsRealFrame(bool realFrame) {
	frameInfo->setIsRealFrame(realFrame);
}

void SimInfo::setCurrentFrame(unsigned char currentFrame) {
	frameInfo->setCurrentFrame(currentFrame);
}
