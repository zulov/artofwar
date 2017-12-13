#pragma once
#include <chrono>
#include <string>
#include <iostream>


struct loading
{
	loading() {
		reset(1);
	}

	~loading() {
	}

	float getProgres() {
		return currentStage / stagesNumber;
	}

	void reset(int stages) {
		stagesNumber = stages;
		currentStage = 0;
		sth = nullptr;
		start = std::chrono::system_clock::now();
	}

	void reset(int stages, std::string _msg) {
		msg = _msg;
		reset(stages);
	}

	void inc() {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
		std::cout << currentStage << " complated (" << msg.c_str() << ") at " << duration.count() << std::endl;
		start = std::chrono::system_clock::now();
		++currentStage;
	}

	void inc(std::string _msg) {
		inc();
		msg = _msg;
	}

	void* sth;
	int currentStage;
private:
	std::string msg = "";
	float stagesNumber;
	std::chrono::system_clock::time_point start;
};