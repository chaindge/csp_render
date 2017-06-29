#ifndef SIMPLERANDOMIZER_H
#define SIMPLERANDOMIZER_H

#include <random>

class SimpleRandomizer
{
public:
	/**
	*	rand double from [0,1]
	*/
	inline static double rand_float()
	{
		std::uniform_real_distribution<double> dis(0.f, 1.f);
		return dis(generator);
	}

	/**
	*	rand double rrom [-1,1]
	*/
	inline static double rand_clamped() {
		return rand_float() - rand_float();
	}

	/**
	*	rand double from [x,y]
	*/
	inline static double rand_in_range(double x, double y) {
		return x + rand_float() * (y - x);
	}

	/**
	*	rand bool
	*/
	inline static bool rand_bool() {
		return rand_float() > 0.5;
	}

	/**
	*	rand int from [imin,imax]
	*/
	inline static int rand_int(int imin, int imax)
	{
		std::uniform_int_distribution<int> dis(imin, imax);
		return dis(generator);
	}

private:
	static std::default_random_engine generator;
};

#endif