#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <utility>


struct Progress {
	Progress(unsigned char stages, bool ifPrint = true): stagesNumber(stages), ifPrint(ifPrint) {
		reset();
	}

	Progress(const Progress&) = delete;

	~Progress() = default;

	float getProgress() const {
		return currentStage / (float)stagesNumber;
	}

	void reset() {
		currentStage = 0;
		startOfStage = std::chrono::system_clock::now();
		startOfProgress = startOfStage;
	}

	void reset(std::string _msg) {
		msg = std::move(_msg);
		reset();
	}

	void inc(std::string _msg) {
		if (ifPrint) {
			auto now = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startOfStage);
			std::cout << (int)currentStage << " /" << (int)stagesNumber << " (" << msg.c_str() << ") at " << duration.count()
					  << " ms" << std::endl;
			startOfStage = std::chrono::system_clock::now();
			msg = std::move(_msg);
			if (currentStage == stagesNumber) {
				auto durationWhole = std::chrono::duration_cast<std::chrono::milliseconds>(now - startOfProgress);
				std::cout << "Ended in " << durationWhole.count() << " ms" << std::endl;
			}
		}

		inc();
	}

	void inc() {
		++currentStage;
	}

	unsigned char currentStage;
	const unsigned char stagesNumber;
private:
	const bool ifPrint;
	std::string msg = "";
	std::chrono::system_clock::time_point startOfStage;
	std::chrono::system_clock::time_point startOfProgress;
};
