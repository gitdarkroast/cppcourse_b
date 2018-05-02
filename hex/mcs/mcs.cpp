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

class gen
{
private:
	double x;
	double incr;
public:
	gen(double x_zero, double increment) :x(x_zero), incr(increment) {};
	double operator()() { x += incr; return x * x; };
};

double integrate(gen g, int n)
{
	std::vector<double> fx(n);

	std::generate(fx.begin(), fx.end(), g);
	return (std::accumulate(fx.begin(), fx.end(), 0.0)/n);
}

class duo
{
protected:
	double first;
	double second;
public:
	duo() :first(0.0), second(0.0) {}
	duo(double x, double y) :first(x), second(y) {}
	void set_first(double d) { first = d; }
	void set_second(double d) { second = d; }
	double get_first() { return first; }
	double get_second() { return second; }
};

class point :public duo
{
public:
	point() :duo() {}
	point(double x, double y = 0.0) :duo(x, y) {}
	virtual double length() { return std::sqrt(first*first + second * second); }
};

class point3d final:public point
{
private:
	double z;
public:
	point3d() :point(), z(0.0) {}
	double length() { return std::sqrt(first*first + second * second + z * z); }
	void set_z(double d) { z = d; }
};

int main()
{
	Timer t;
	const int n{ 10000 };

	gen g{ 0.0, 1.0 / n };

	std::cout << "Integration program x**2" << std::endl;
	std::cout << integrate(g, n) << std::endl;

	point q;
	q.set_first(3.0);
	q.set_second(4.0);
	std::cout << q.get_first() << ", " << q.get_second() << std::endl;
	std::cout << q.length() << std::endl;

	Board b(10);

	std::cout << b.isCorner(Point(1, 1));
	std::cout << b.isCorner(Point(0, 0));
    return 0;
}
