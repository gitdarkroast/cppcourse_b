// hex.cpp : Defines the entry point for the console application.
//
/*
Assignment Objectives:
Your program should use a graph representation and treat the game
as a path finding problem.Each internal node(hexagon) has six neighbors
– so each would have 6 edges.The corners and the edges are special.A corner
has either two or three neighbors and a non - corner edge has 4 neighbors.

The player should be able to interact with the program and choose its “color”
with blue going first.The program should have a convenient interface for
entering a move, displaying the board, and then making its own move.The 
program should determine when the game is over and announce the winner.

A simple strategy could be to extend your current longest path or to 
block your opponent’s longest path.A very dumb strategy would be to 
play on an empty hexagon at random.

A simple board display would be to have an 11 x 11 printout with B, R, 
or a blank in each position.A simple way to input a move would be to have 
the player enter an(i, j) coordinate corresponding to a currently empty 
hexagon and have the program check that this is legal and if not ask for another choice.

HW 4 expectations:
 - Be able to draw the board using ASCII symbols and a given size, such as 7 by 7 or 11 by 11.
 - Input a move and determine if a move is legal.
 - Determine who won.
*/
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <limits>
//#include "hex.h"

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
    void dellEdge(int i, int j) { m_matrix[i][j] = m_matrix[j][i] = INT_MAX; }

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
    Board(int size = DefaultBoardSize) : m_size{ size }, m_num_of_turns{ 0 },
        m_last_sim{ Point{ INT_MAX, INT_MAX } }, m_last_user{ Point{ INT_MAX, INT_MAX } },
        m_graph{ m_size }
    {
        // We start with all the nodes being unoccuppied.
        m_nodes.assign(m_size*m_size, Color::COLOR_BLANK);
    }
    int get_node(Point p) const { return std::get<0>(p) * m_size + std::get<1>(p); }
    Point get_point(int n) const { return Point{n / m_size, n % m_size}; }
    bool isTopLeftCorner(const Point& p) { return std::get<0>(p) == 0 && std::get<1>(p) == 0; }
    bool isTopRightCorner(const Point& p) { return std::get<0>(p) == 0 && std::get<1>(p) == m_size - 1; };
    bool isBotLeftCorner(const Point& p) { return std::get<0>(p) == m_size - 1 && std::get<1>(p) == 0; }
    bool isBotRightCorner(const Point& p) { return std::get<0>(p) == m_size - 1 && std::get<1>(p) == m_size - 1; }
    bool isCorner(const Point& p) { return isTopLeftCorner(p) || isTopRightCorner(p) || isBotLeftCorner(p) || isBotRightCorner(p); }
    int  getTopLeftNode(Point& p) { return get_node(Point{ std::get<0>(p) - 1, std::get<1>(p) }); }
    int  getTopRightNode(Point& p) { return get_node(Point{ std::get<0>(p) - 1, std::get<1>(p) + 1 }); }
    int  getLeftNode(Point& p) { return get_node(Point{ std::get<0>(p), std::get<1>(p) - 1 }); }
    int  getRightNode(Point& p) { return get_node(Point{ std::get<0>(p), std::get<1>(p) + 1 }); }
    int  getBotLeftNode(Point& p) { return get_node(Point{ std::get<0>(p) + 1, std::get<1>(p) -1 }); }
    int  getBotRightNode(Point& p) { return get_node(Point{ std::get<0>(p) + 1, std::get<1>(p) }); }
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
    void generate2();

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
    Hex(int size) : m_size{ size }, m_board{ size } {}
    bool winner();
    void play();
    Point getUserMove();
    Point getComputerMove();

};


// End of hex header

// Implementation
std::ostream& operator<<(std::ostream& out, const Point& p)
{
    out << "(" << std::get<0>(p) << ", " << std::get<1>(p) << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Graph& g)
{
    for (int i = 0; i < g.m_matrix.size(); ++i)
    {
        out << "Node " << i << " : ";
        for (int j = 0; j < g.m_matrix[i].size(); ++j)
        {
            
            out << (1 == g.m_matrix[i][j] ? " 1 " : " - ");
        }
        out << std::endl;
    }
    return out;
}

// Graph methods
Graph::Graph(int size) : m_size{ size }
{
    m_matrix.assign(m_size, std::vector<int>(m_size));
    for (int i = 0; i < m_size; ++i)
    {
        for (int j = 0; j < m_size; ++j)
        {
            m_matrix[i][j] = m_matrix[j][i] = INT_MAX;
        }
    }

}

// Board methods


// Determine is a point within board boundary
bool Board::isWithinBoundary(Point p)
{
	int x = std::get<0>(p);
	int y = std::get<1>(p);

	return (x >= 0 && x < m_size) && (y >= 0 && y < m_size);
}

// Determine if a mode is legal
// - point must be within the board boundary
// - point must map onto a node that is not occupied
bool Board::isAvailable(Point p)
{

	return isWithinBoundary(p) && isBlank(get_node(p));
}

// Make a move on the board
bool Board::makeMove(Point p, Color c)
{
	bool bResult = isAvailable(p);
	if (bResult)
	{
		setColor(get_node(p), c);
        if (Color::COLOR_BLUE == c)
        {
            m_graph.dellEdge(std::get<0>(p), std::get<1>(p));
        }
	}
	return bResult;
}

void Board::generate2()
{
    int n = m_size;
    for (int i = 0; i<n*n; i++)
    {
        if (!((i + 1) % n == 0) && !(i < n))
            connect(i, i - n + 1);
        if (!(i%n == 0) && !(i + n >= n * n)/*not on left edge and not bottom row*/)
            connect(i, i + n - 1);
        if (!(i < n)/*not top row*/)
            connect(i, i - n);
        else
            connect(i, n*n);
        if (!((i + 1) % n == 0)/*not on right edge */)
            connect(i, i + 1);
        else
            connect(i, n*n+1);
        if (!(i%n == 0)/*not on left edge */)
            connect(i, i - 1);
        else
            connect(i, n*n+3);
        if (!(i + n >= n * n)/*not bottom row*/)
            connect(i, i + n);
        else
            connect(i, n*n+2);

    }
}
// Generate the graph for the computer simulation
// This is really a brute force method of generating a connected
// graph.  Some investment of time is needed to optimise algorithm.
void Board::generate()
{
    /*
     Will use the odd-r horizontal layout:
     https://www.redblobgames.com/grids/hexagons/#coordinates
     A Hex board in terms of nodes:

     0 1 2 3 4
      5 6 7 8 9
     10 11 12 13 14
      15 16 17 18 19
     20 21 22 23 24

     The above shows that we have to take care of the column movement
     for the alternating row.

    */
    for (int i = 0; i < m_size*m_size; ++i)
    {
        Point p = get_point(i);
        int row = std::get<0>(p);
        int col = std::get<1>(p);
        // Let's process the inner nodes first
        if(!isTop(p)
            && !isBottom(p)
            && !isLHS(p)
            && !isRHS(p)
            )
        {
            int left = get_node(Point{ row, col - 1 });
            int right = get_node(Point{ row, col + 1 });
            connect(i, left);
            connect(i, right);
            if (1 == (row % 2))
            {
                int top_left = get_node(Point{ row - 1, col });
                int top_right = get_node(Point{ row - 1, col + 1 });
                int bot_left = get_node(Point{ row + 1, col });
                int bot_right = get_node(Point{ row + 1, col + 1 });
                connect(i, top_left);
                connect(i, top_right);
                connect(i, bot_left);
                connect(i, bot_right);
            }
            else
            {
                int top_left = get_node(Point{ row - 1, col - 1 });
                int top_right = get_node(Point{ row - 1, col });
                int bot_left = get_node(Point{ row + 1, col - 1});
                int bot_right = get_node(Point{ row + 1, col });
                connect(i, top_left);
                connect(i, top_right);
                connect(i, bot_left);
                connect(i, bot_right);
            }
        }
        // The top row
        if (isTop(p) && !isCorner(p))
        {
            int left = get_node(Point{ row, col - 1 });
            int right = get_node(Point{ row, col + 1 });
            int bot_left = get_node(Point{ row + 1, col - 1 });
            int bot_right = get_node(Point{ row + 1, col });
            connect(i, left);
            connect(i, right);
            connect(i, bot_left);
            connect(i, bot_right);
            
        }
        // The bottom row
        if (isBottom(p) && !isCorner(p))
        {
            int left = get_node(Point{ row, col - 1 });
            int right = get_node(Point{ row, col + 1 });
            int top_left = get_node(Point{ row - 1, col });
            int top_right = get_node(Point{ row - 1, col + 1 });
            connect(i, top_left);
            connect(i, top_right);
            connect(i, left);
            connect(i, right);
            
        }
        // Left side of the board
        if(isLHS(p) && !isCorner(p))
        {
            int right = get_node(Point{ row, col + 1 });
            connect(i, right);
            if (1 == (row % 2))
            {
                int top_left = get_node(Point{ row - 1, col });
                int top_right = get_node(Point{ row - 1, col + 1 });
                int bot_left = get_node(Point{ row + 1, col });
                int bot_right = get_node(Point{ row + 1, col + 1 });
                connect(i, top_right);
                connect(i, bot_right);
                connect(i, top_left);
                connect(i, bot_left);
            }
            else
            {
                int top_right = get_node(Point{ row - 1, col });
                int bot_right = get_node(Point{ row + 1, col });
                connect(i, top_right);
                connect(i, bot_right);
            }
        }
        // Right side of the board
        if(isRHS(p) && !isCorner(p))
        {
            int left = get_node(Point{ row, col - 1 });
            connect(i, left);
            if (1 == (row % 2))
            {
                int top_left = get_node(Point{ row - 1, col });
                int bot_left = get_node(Point{ row + 1, col });
                connect(i, top_left);
                connect(i, bot_left);
            }
            else
            {
                int top_right = get_node(Point{ row - 1, col });
                int top_left = get_node(Point{ row - 1, col - 1});
                int bot_left = get_node(Point{ row + 1, col - 1 });
                int bot_right = get_node(Point{ row + 1, col });
                connect(i, top_right);
                connect(i, top_left);
                connect(i, bot_right);
                connect(i, bot_left);
            }
        }
    }
}


// This ugly little overloaded output operator is to print the board in ascii.
std::ostream& operator<<(std::ostream& out, const Board& b)
{
	int size = b.m_size;
    // For debugging, dump the graph matrix:
    out << "Current graph used by the computer: " << std::endl;
    out << b.m_graph;
    // First an informative header
    out << "User move: " << b.m_last_user << ", computer move: " << b.m_last_sim << std::endl;
	// The outer loop controls the vertical dimension
	for (int i = 0; i < size; i++)
	{
		// Print out the leading spaces
		for (int s = 0; s < i; ++s)
		{
			out << " ";
		}
		// The inner loop controls the horizontal dimension
		for (int j = 0; j < size; ++j)
		{
			int square = b.get_node(Point(i, j));
			out << static_cast<std::string>(b.isOccupied(square) ? ((b.getColor(square) == Color::COLOR_BLUE) ? " B " : " R ") : " . ");
			// Avoid the dangling "-"
			if (j != size - 1)
			{
				out << "-";
			}
		}
		out << std::endl;
		// Now print out the "/ \" padding line.
		// To avoid the dangling "/ \" line we go to (size -1)
		if (i != size - 1)
		{
			// Print out the leading spaces
			for (int s = 0; s < i + 2; ++s)
			{
				out << " ";
			}
			//out << " ";
			for (int j = 0; j < size; ++j)
			{
				// Avoid the dangling "/"
				if (j != size - 1)
				{
					out << "\\ / ";
				}
				else
				{
					out << "\\" << std::endl;
				}
			}
		}
	}
	return out;
}

// Hex Implementation

// Determines if there is a winner
bool Hex::winner()
{
	return false;
}

// Gets the user's move
Point Hex::getUserMove()
{
	Point p{ INT_MAX, INT_MAX };
	std::string input = "";

	while (true)
	{
 
		std::cout << "Please enter the move co-ordinates: ";
		std::cout.flush();
		std::getline(std::cin, input);
		// Trying to catch a Control-C - a way to end the loop
		if (std::cin.fail() || std::cin.eof())
		{
			break;
		}
		std::istringstream line_stream(input);
		int x;
		if (!(line_stream >> x))
		{
			std::cout << "Invalid entry, expecting a number." << std::endl;
			continue;
		}
		int y;
		if (!(line_stream >> y))
		{
			std::cout << "Invalid entry, expecting a number." << std::endl;
			continue;
		}
		// So we have valid x,y co-ordinates
		std::get<0>(p) = x;
		std::get<1>(p) = y;
		if (m_board.isAvailable(p))
		{
			break;
		}
		std::cout << "Coordinates are either out of bounds or the square is already occupied." << std::endl;
	}

	return p;
}

// Determine the computer's move
Point Hex::getComputerMove()
{
	Point p{ INT_MAX, INT_MAX };


	return p;
}

// Main play loop
void Hex::play()
{
    // Generate the disconnected graph for the computer
    m_board.generate();
    std::cout << "Start position:" << std::endl;
    std::cout << m_board;
	while (!winner())
	{
		Point p = getUserMove();
		if (p != Point{ INT_MAX, INT_MAX })
		{
			m_board.makeMove(p, Color::COLOR_BLUE);
            m_board.setUserMove(p);
			std::cout << m_board;
		}
		else
		{
			// Attempting to quit the game.
			break;
		}
	}
}

// Main entry
/*
int main()
{
	
	Hex h{ 4 };

	h.play();
    return 0;
}
*/
