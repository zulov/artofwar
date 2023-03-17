#include "SimInfo.h"

#include "FrameInfo.h"

SimInfo::SimInfo() {
	frameInfo = new FrameInfo();
}

SimInfo::~SimInfo() { delete frameInfo; }

void SimInfo::setIsRealFrame(bool realFrame) const {
	frameInfo->setIsRealFrame(realFrame);
}

void SimInfo::setFrame(unsigned char currentFrame, unsigned secondsElapsed) const {
	frameInfo->setCurrentFrame(currentFrame);
	frameInfo->setSeconds(secondsElapsed);
}
