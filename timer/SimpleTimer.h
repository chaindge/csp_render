#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H
#include<chrono>
#include "Regulator.h"

#define _SimpleTimer SimpleTimer::getInstance()

class SimpleTimer
{
private:
	SimpleTimer()
		: m_StartTime(std::chrono::high_resolution_clock::now())
		, m_updator(60.f) // default fps is 60
	{}

	SimpleTimer(const SimpleTimer&) = delete;
	SimpleTimer& operator=(const SimpleTimer&) = delete;
public:

	static SimpleTimer* getInstance() {
		static SimpleTimer instance;
		return &instance;
	}

	inline void reset() { m_StartTime = std::chrono::high_resolution_clock::now(); }

	// return seconds
	inline double elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::duration<double> >(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	// return milliseconds
	inline int64_t elapsed_milli() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	//return micro seconds
	inline int64_t elapsed_micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	//return nano seconds
	inline int64_t elapsed_nano() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	// return seconds
	inline int64_t elapsed_seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	// return minutes
	inline int64_t elapsed_minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	// return hours
	inline int64_t elapsed_hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	}

	// return time elapsed since last call to this funtion in seconds
	double time_elapsed() const {
		static double _last_time = elapsed();

		double current = elapsed();
		double delta = current - _last_time;
		_last_time = current;
		return delta;
	}

	// is the updator ready for next update of 
	inline bool ready_for_next_update() {
		return m_updator.isReady();
	}

	// set updator fps
	inline void set_updator_fps(float fps) {
		m_updator.setFPS(fps);
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	Regulator m_updator;
};

#endif // !SIMPLETIMER_H
