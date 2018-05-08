// mcs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <ctime>
#include <chrono>

#include "../hex/hex.h"

class Timer
{
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1>>;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now())
	{}

	void reset()
	{
		m_beg = clock_t::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};


int main()
{
	Timer t;
	

	Hex h(10);


	h.makeMove(Point(0, 0), Color::COLOR_RED);
	h.makeMove(Point(1, 1), Color::COLOR_RED);
	h.makeMove(Point(2, 2), Color::COLOR_RED);
	h.makeMove(Point(3, 3), Color::COLOR_RED);
	h.makeMove(Point(4, 4), Color::COLOR_RED);
	h.makeMove(Point(5, 5), Color::COLOR_RED);
	h.makeMove(Point(6, 6), Color::COLOR_RED);
	h.makeMove(Point(0, 1), Color::COLOR_BLUE);
	h.makeMove(Point(1, 2), Color::COLOR_BLUE);
	h.makeMove(Point(2, 3), Color::COLOR_BLUE);
	h.makeMove(Point(3, 4), Color::COLOR_BLUE);
	h.makeMove(Point(4, 5), Color::COLOR_BLUE);
	h.makeMove(Point(5, 6), Color::COLOR_BLUE);
	h.makeMove(Point(6, 7), Color::COLOR_BLUE);

	std::cout << h;

    return 0;
}
