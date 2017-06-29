#ifndef REGULATOR_H
#define REGULATOR_H

#include <chrono>

#include "../math/csp_math.h"
#include "SimpleRandomizer.h"

class Regulator
{
private:
	float m_fUpdatePeriod; // milliseconds

	std::chrono::time_point<std::chrono::high_resolution_clock> m_NextUpdateTime;

public:
	Regulator(float NumUpdatesPerSecondRqd)
	{
		setFPS(NumUpdatesPerSecondRqd);
	}

	void setFPS(float fps)
	{
		auto first_rand = std::chrono::milliseconds(int(SimpleRandomizer::rand_float() * 1000.0));
		m_NextUpdateTime = std::chrono::high_resolution_clock::now() + first_rand;

		if (fps > 0) {
			m_fUpdatePeriod = 1000.f / fps;
		}

		else if (float_equal(fps, 0.f)) {
			m_fUpdatePeriod = 0.f;
		}

		else if (fps < 0.f) {
			m_fUpdatePeriod = -1.f;
		}
	}

	bool isReady()
	{
		if (float_equal(m_fUpdatePeriod, 0.f))return true;

		if (m_fUpdatePeriod < 0.f) return false;

		auto CurrentTime = std::chrono::high_resolution_clock::now();

		static const float UpdatePeriodVariator = 10.f;

		if (CurrentTime > m_NextUpdateTime)
		{
			std::chrono::milliseconds update_period(int(m_fUpdatePeriod +
				SimpleRandomizer::rand_in_range(-UpdatePeriodVariator, UpdatePeriodVariator)));

			m_NextUpdateTime = CurrentTime + update_period;
			
			return true;
		}

		return false;
	}
};

#endif