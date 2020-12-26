#pragma once
#include <stdio.h>
#include <chrono>

class StackLogger
{
public:
	StackLogger::StackLogger(const char* name)
	:	m_Start(std::chrono::high_resolution_clock::now()),
		m_Name(name)
	{
	}

	StackLogger::~StackLogger()
	{
		auto end = std::chrono::high_resolution_clock::now();
		float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Start).count();
		std::printf("[%s] Passed: %.3fms\n", m_Name, elapsed_ms);
	}

private:
	std::chrono::steady_clock::time_point m_Start;
	const char* m_Name;
};