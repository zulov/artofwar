#pragma once
struct FrameInfo {

	void setIsRealFrame(bool realFrame) { this->realFrame = realFrame; }

	void set(unsigned char currentFrame, unsigned int seconds) {
		this->currentFrame = currentFrame;
		this->realFrame = true;
		this->seconds = seconds;
	}

	unsigned char getCurrentFrame() const { return currentFrame; }
	bool isRealFrame() const { return realFrame; }
	unsigned int getSeconds() const { return seconds; }
private:
	unsigned int seconds;
	unsigned char currentFrame;
	bool realFrame;
};
