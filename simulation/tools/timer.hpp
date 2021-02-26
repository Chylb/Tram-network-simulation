#include <iostream>
#include <chrono>

struct Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;
	std::string message;

	Timer(std::string message) {
		start = std::chrono::high_resolution_clock::now();
		this->message = message;
	}

	void finish() {
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float ms = duration.count() * 1000.0f;
		std::cout << message << ": " << ms << " ms" << std::endl;
	}
};