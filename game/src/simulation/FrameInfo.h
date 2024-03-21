#pragma once

constexpr unsigned int SECONDS_IN_MONTH = 5;

struct FrameInfo {
	void resetRealFrame() { realFrame = false; }

	void set(unsigned char currentFrame, unsigned int seconds) {
		this->currentFrame = currentFrame;
		this->realFrame = true;
		this->seconds = seconds;
	}

	unsigned char getCurrentFrame() const { return currentFrame; }
	bool isRealFrame() const { return realFrame; }
	unsigned int getSeconds() const { return seconds; }

	std::pair<unsigned char, unsigned short> getDate() const {
		const auto allMonths = seconds / SECONDS_IN_MONTH;
		return {allMonths % 12, allMonths / 12};
	}

	std::tuple<unsigned char, unsigned char, unsigned char> getTime() const {
		return std::make_tuple(seconds / 3600, (seconds / 60) % 60, seconds % 60);
	}

private:
	unsigned int seconds;
	unsigned char currentFrame;
	bool realFrame;
};
