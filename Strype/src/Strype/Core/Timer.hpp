#pragma once

#include <chrono>
#include <glm/glm.hpp>

class Timer
{
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	Timer() 
	{
		Reset();
	}

	void Reset() 
	{
		m_StartTime = Clock::now();
	}

	float ElapsedMinutes() const
	{
		return std::chrono::duration<float, std::ratio<60>>(Clock::now() - m_StartTime).count();
	}

	float ElapsedSeconds() const 
	{
		return std::chrono::duration<float>(Clock::now() - m_StartTime).count();
	}

	long long ElapsedMilliseconds() const 
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - m_StartTime).count();
	}

	long long ElapsedMicroseconds() const 
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_StartTime).count();
	}

	std::string FormatTime() const
	{
		return fmt::format("{}:{}.{}", glm::floor(ElapsedMinutes()), glm::floor(ElapsedSeconds()), glm::floor(ElapsedMilliseconds()));
	}

private:
	TimePoint m_StartTime;
};
