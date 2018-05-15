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
typedef std::tuple<unsigned int, unsigned int> Point;
typedef std::vector<std::vector<int>> Matrix;

// Grapg ADT
class Graph {
private:
    int m_size;
    std::vector < std::vector<int>> m_matrix;
public:
    Graph(int size);
    void addEdge(int i, int j) { m_matrix[i][j] = m_matrix[j][i] = 1; }

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


// Board ADT
class Board
{
private:
	int m_size;
    Point m_last_user;
    Point m_last_sim;
    int m_num_of_turns;
	std::vector<Color> m_nodes;
    Graph m_graph;

public:
    Board(int size = DefaultBoardSize) : m_size{ size }, m_num_of_turns{0},
        m_last_sim{ Point{INT_MAX, INT_MAX} }, m_last_user{ Point{INT_MAX, INT_MAX} },
        m_graph{ m_size*m_size }
	{
		// We start with all the nodes being unoccuppied.
		m_nodes.assign(m_size*m_size, Color::COLOR_BLANK);
	}
	int get_node(Point p) const { return std::get<0>(p) * m_size + std::get<1>(p); }
	Point get_point(int n) const { return Point(n / m_size, n % m_size); }
	bool isCorner(const Point& p);
    bool isTop(const Point& p) { return 0 == std::get<0>(p); }
    bool isBottom(const Point& p) { return (m_size - 1) == std::get<0>(p); }
    bool isLHS(const Point& p) { return 0 == std::get<1>(p); }
    bool isRHS(const Point& p) { return (m_size - 1) == std::get<1>(p); }
	bool isBlank(int n) const { return m_nodes[n] == Color::COLOR_BLANK; }
	bool isOccupied(int n) const { return !isBlank(n); }
	Color getColor(int n) const { return m_nodes[n]; }
    void connect(int i, int j) { m_graph.addEdge(i, j); }
	void setColor(int n, Color c) { m_nodes[n] = c; }
    void setUserMove(Point p) { m_last_user = p; }
    void setSimMove(Point p) { m_last_sim = p; }
	bool isWithinBoundary(Point p);
	bool isAvailable(Point p);
	bool makeMove(Point p, Color c);
	void generate();

	friend std::ostream& operator<<(std::ostream& out, const Board& b);
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
	Board m_board;
public:
	Hex(int size) : m_size{size}, m_board{size} {}
	bool winner();
	void play();
	Point getUserMove();
	Point getComputerMove();

};


// End of hex header
