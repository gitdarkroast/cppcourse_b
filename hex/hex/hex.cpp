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
#include <string>
#include "hex.h"

// Implementation

// Graph methods
Graph::Graph(int size)
{
	m_size = size;
	m_matrix.resize(m_size);
	// We start with disconnected graph
	for (int i = 0; i < m_size; ++i)
	{
		m_matrix[i].assign(m_size, 0);
	}
}

// Board methods

// Check to see if the point given is a corner.
bool Board::isCorner(const Point& p)
{
	bool top_left = std::get<0>(p) == 0 && std::get<1>(p) == 0;
	bool top_right = std::get<0>(p) == 0 && std::get<1>(p) == m_size - 1;
	bool bot_left = std::get<0>(p) == 0 && std::get<1>(p) == m_size -1;
	bool bot_right = std::get<0>(p) == m_size - 1 && std::get<1>(p) == m_size -1;

	return top_left || top_right || bot_left || bot_right;
}

// Determine is a point within board boundary
bool Board::isWithinBoundary(Point p)
{
	int x = std::get<0>(p);
	int y = std::get<1>(p);

	return (x >= 0 && x < m_size) &&
		(y >= 0 && y < m_size);
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
		m_squares[get_node(p)] = c;
	}
	return bResult;
}

// Set the square to the specified color
bool Board::setSquare(int n, Color c)
{
	bool bResult = isWithinBoundary(get_square(n));
	if (bResult)
	{
		m_squares[n] = c;
	}
	return bResult;
}


// Hex Implementation

// This ugly little overloaded output operator is to print the board in ascii.
std::ostream& operator<<(std::ostream& out, const Hex& h)
{
	int size = h.m_size;
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
			int square = h.m_board.get_node(Point(i, j));
			out << static_cast<std::string>(h.m_board.isOccupied(square) ? ((h.m_board.getColor(square) == Color::COLOR_BLUE) ? " B " : " R ") : " . ");
			// Avoid the dangling "-"
			if (j != size - 1)
			{
				out << "-";
			}
		}
		out << std::endl;
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
