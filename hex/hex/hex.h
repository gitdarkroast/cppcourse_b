#pragma once
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>

// For the purpose of the assignment, definition and implementation
// are in one file.

//  Traditional Header definitions here
static const int DefaultBoardSize{ 11 };
enum class Color : std::int8_t { COLOR_BLANK = 0, COLOR_BLUE = 1, COLOR_RED = 2 };
typedef std::tuple<int, int> Point;

// Graph ADT
class Graph
{
private:
	int m_size;
	std::vector<std::vector<int>> m_matrix;
public:
	Graph(int size);
	void addEdge(int i, int j, int cost = 1) { m_matrix[i][j] = cost; }
	void delEdge(int i, int j) { m_matrix[i][j] = 0; }

};

class Board
{
private:
	int m_size;
	std::vector<Color> m_squares;
public:
	Board(int size = DefaultBoardSize) : m_size{ size }
	{
		// We start with all the squares being unoccuppied.
		m_squares.assign(m_size*m_size, Color::COLOR_BLANK);
	}
	int get_node(Point p) { return std::get<0>(p) * m_size + std::get<1>(p); }
	Point get_square(int n) { return Point(n / m_size, n % m_size); }
	bool isCorner(const Point& p);
	bool isBlank(int n) { return m_squares[n] == Color::COLOR_BLANK; }
	bool isOccupied(int n) { return !isBlank(n); }
	bool isWithinBoundary(Point p);
	bool isAvailable(Point p);
	bool makeMove(Point p, Color c);
};

// Hex ADT
// Hex is a game
// Hex has a board
// Hex has a players
// Hex has a moves

class Hex
{
private:
	int m_size;
	Graph m_graph{ m_size };
	Board m_board{ m_size };
public:
	Hex(int size);
};


// End of hex header
