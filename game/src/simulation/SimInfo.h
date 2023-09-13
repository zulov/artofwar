#pragma once


struct FrameInfo;

class SimInfo {
public:
	SimInfo();
	~SimInfo();

	const FrameInfo* getFrameInfo() const { return frameInfo; }
	void setIsRealFrame(bool realFrame) const;
	void setFrame(unsigned char currentFrame, unsigned secondsElapsed) const;
private:
	FrameInfo* frameInfo;
};
