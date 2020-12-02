#pragma once
struct FrameInfo {

	void setIsRealFrame(bool realFrame) { this->realFrame = realFrame; }

	void setCurrentFrame(unsigned char currentFrame) {
		this->currentFrame = currentFrame;
		this->realFrame = true;
	}

	void setSeconds(unsigned int seconds) { this->seconds = seconds; }

	char getCurrentFrame() const { return currentFrame; }
	bool isRealFrame() const { return realFrame; }
	unsigned int getSeconds() const { return seconds; }
private:
	bool realFrame;
	unsigned char currentFrame;
	unsigned int seconds;
};
