#pragma once

#include <stdio.h>
#include <chrono>

class StackLogger
{
public:
	StackLogger::StackLogger(const StackLogger&) = delete;
	StackLogger::StackLogger(void(*destroyedCallback)(float elapsed_ms))
	:	m_Start(std::chrono::high_resolution_clock::now()),
		m_DestroyedCallback(destroyedCallback)
	{
	}

	StackLogger::~StackLogger()
	{
		auto end = std::chrono::high_resolution_clock::now();
		float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Start).count();
		if (m_DestroyedCallback)
			m_DestroyedCallback(elapsed_ms);
	}

private:
	std::chrono::steady_clock::time_point m_Start;
	void (*m_DestroyedCallback)(float) = nullptr;
};