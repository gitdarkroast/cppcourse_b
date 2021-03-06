// hex.cpp
/*
Homework expectations:
 - Be able to draw the board using ASCII symbols and a given size, such as 7 by 7 or 11 by 11.
 - Input a move and determine if a move is legal.
 - Determine who won.
*/
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>
#include <sstream>
#include <limits>

// For the purpose of the assignment, definition and implementation
// are in one file.

// Credit: http://www.learncpp.com/cpp-tutorial/8-16-timing-your-code/
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

// A random generator to assist with random int and double values
// Credit: http://www.learncpp.com/cpp-tutorial/59-random-number-generation/
class Random
{
private:
    std::random_device m_rd;
    std::mt19937 m_mersenne;

public:
    Random() {
        m_mersenne = std::mt19937(m_rd());
    };
    double getRandom(double start, double end)
    {
        std::uniform_real_distribution<double> dist{ start, end };

        double result = dist(m_mersenne);
        return result;
    }

    int getRandom(int start, int end)
    {
        std::uniform_int_distribution<int> dist{ start, end };

        int result = dist(m_mersenne);
        return result;
    }
};

//  Traditional Header definitions here
static const int DefaultBoardSize{ 11 };
enum class Color : std::int8_t { COLOR_BLANK = 0, COLOR_BLUE = 1, COLOR_RED = 2 };
typedef std::tuple<unsigned int, unsigned int> Coord;

// Graph ADT
// We use a nxn matrix to represent the graph.
// [i][j] repesents an edge between node i and node j.
// Initially all edges have a cost of INT_MAX which means
// not connected.
class Graph {
private:
    int m_size;
    std::vector<bool> m_visited;
    std::vector<int> m_distance;
    std::vector< std::vector<int>> m_matrix;
public:
    Graph(int size=DefaultBoardSize) :m_size{ size }
    {
        // Allocate the correct amount of space.
        m_matrix.assign(m_size, std::vector<int>(m_size));
        for (int i = 0; i < m_size; ++i)
        {
            for (int j = 0; j < m_size; ++j)
            {
                // Initially the nodes are not connected.
                m_matrix[i][j] = m_matrix[j][i] = INT_MAX;
            }
        }
    }
    void addEdge(int i, int j, int cost=1) { m_matrix[i][j] = m_matrix[j][i] = cost; }
    void delEdge(int i, int j) { m_matrix[i][j] = m_matrix[j][i] = INT_MAX; }
    int  getCost(int i, int j) const { return m_matrix[i][j]; }
    bool hasEdge(int i, int j) const { return getCost(i, j) != INT_MAX; }
    // Determine if there is a path between src and dst.
    bool path(int src, int dst);
    std::vector<int> neighbors(int i);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


// Board ADT
class Board
{
private:
    int m_size;
    std::vector<Color> m_nodes;

    std::vector<int> m_east;
    std::vector<int> m_west;
    std::vector<int> m_north;
    std::vector<int> m_south;

public:
    Board(int size = DefaultBoardSize);
    int get_node(Coord p) const { return std::get<0>(p) * m_size + std::get<1>(p); }
    Coord get_point(int n) const { return Coord{n / m_size, n % m_size}; }
    bool isTopLeftCorner(const Coord& p) { return std::get<0>(p) == 0 && std::get<1>(p) == 0; }
    bool isTopRightCorner(const Coord& p) { return std::get<0>(p) == 0 && std::get<1>(p) == m_size - 1; };
    bool isBotLeftCorner(const Coord& p) { return std::get<0>(p) == m_size - 1 && std::get<1>(p) == 0; }
    bool isBotRightCorner(const Coord& p) { return std::get<0>(p) == m_size - 1 && std::get<1>(p) == m_size - 1; }
    bool isCorner(const Coord& p) { return isTopLeftCorner(p) || isTopRightCorner(p) || isBotLeftCorner(p) || isBotRightCorner(p); }
    bool isTop(const Coord& p) { return 0 == std::get<0>(p); }
    bool isBottom(const Coord& p) { return (m_size - 1) == std::get<0>(p); }
    bool isLHS(const Coord& p) { return 0 == std::get<1>(p); }
    bool isRHS(const Coord& p) { return (m_size - 1) == std::get<1>(p); }
    bool isInner(const Coord& p) { return !isTop(p) && !isBottom(p) && !isLHS(p) && !isRHS(p); }
    bool isBlank(int n) const { return m_nodes[n] == Color::COLOR_BLANK; }
    bool isOccupied(int n) const { return !isBlank(n); }
    Color getColor(int n) const { return m_nodes[n]; }
    void setColor(int n, Color c) { m_nodes[n] = c; }
    std::vector<int> east() { return m_east; }
    std::vector<int> west() { return m_west; }
    std::vector<int> north() { return m_north; }
    std::vector<int> south() { return m_south; }
    bool isWithinBoundary(Coord p);
    bool isAvailable(Coord p) { return isWithinBoundary(p) && isBlank(get_node(p)); }
    std::vector<int> getNeighbor(int n, Color c);

    friend std::ostream& operator<<(std::ostream& out, const Board& b);
};

enum class PlayerType : std::int8_t { HUMAN = 0, COMPUTER };
class Player
{
private:
    PlayerType m_type;
    Graph m_graph;
    std::vector<Coord> m_moves;
public:
    Player(PlayerType t, int size) : m_type{ t }, m_graph{ size*size } {}
    void setMove(Coord p) { m_moves.push_back(p); }
    Graph graph() { return m_graph; }
    std::vector<Coord> moves() const { return m_moves; }
};

// Do we support the pie rule feature
static const bool enable_pie_rule{ false };

// Hex ADT
// Hex is a game
// Hex has a board
// Hex has a players
// Hex has a moves

class HexGame
{
private:
    int m_size;
    Player m_first_player{ PlayerType::HUMAN, m_size };
    Player m_second_player{ PlayerType::COMPUTER, m_size };
    Board m_board;
public:
    HexGame(int size) : m_size{ size }, m_board{ size } {}
    void play();
    Coord getUserMove();
    Coord getAIMove();
    bool checkForPath(std::vector<int> src, std::vector<int> dst, Graph g, Color c);

};

// End of hex header

// Implementation

// Output operator for printing a Coord
std::ostream& operator<<(std::ostream& out, const Coord& p)
{
    out << "(" << std::get<0>(p) << ", " << std::get<1>(p) << ")";
    return out;
}

// Output operator for printing a Graph
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

std::ostream& operator<<(std::ostream& out, std::vector<int>& v)
{
    out << "[ ";
    for (auto x : v)
    {
        out << x << " ";
    }
    out << "]";
    return out;
}

// Find the neighbors for a specific node
std::vector<int> Graph::neighbors(int n)
{
	// Declare an empty vector
	std::vector<int> neighbor;
	for (int i = 0; i < m_matrix.size(); ++i)
    {   
		if (m_matrix[n][i] != INT_MAX)
		{
			neighbor.push_back(i);
		}
	}
	return neighbor;
}

// Check if there is a path between the provided
// source and destination
bool Graph::path(int src, int dst)
{
    if (src == dst)
    {
        return true;
    }

    enum class State { UNVISITED, VISITED, VISITING };
    std::priority_queue<int> node_queue;
    std::vector<State> visit_state;

    visit_state.assign(m_size, State::UNVISITED);

    visit_state[src] = State::VISITING;

    node_queue.push(src);

    while (!node_queue.empty())
    {
        int node = node_queue.top();
        node_queue.pop();
        for (auto& n : neighbors(node))
        {
            if (State::UNVISITED == visit_state[n])
            {
                if (n == dst)
                {
                    return true;
                }
                else
                {
                    visit_state[n] = State::VISITING;
                    node_queue.push(n);
                }
            }
        }
        visit_state[node] = State::VISITED;
    }
    return false;
}

// Board methods
Board::Board(int size) : m_size{ size }
{
    // We start with all the nodes being unoccuppied.
    m_nodes.assign(m_size*m_size, Color::COLOR_BLANK);
    // The following containers contain nodes for the start
    // and end points for the players.
    m_east.resize(m_size);
    m_west.resize(m_size);
    m_north.resize(m_size);
    m_south.resize(m_size);
    for (int i = 0, j = 0; i < (m_size*m_size); ++j, i += m_size)
    {
        m_east[j] = i;                          // The left side of the board
        m_west[j] = i + (m_size - 1);           // The right side of the board
        m_north[j] = j;                         // The top row
        m_south[j] = (m_size*m_size) - (j + 1); // The bottom row
    }
}


// Determine if a point within board boundary
bool Board::isWithinBoundary(Coord p)
{
	int x = std::get<0>(p);
	int y = std::get<1>(p);

	return (x >= 0 && x < m_size) && (y >= 0 && y < m_size);
}

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

// The following container allows us to determine the offset required to
// determine the neighbor nodes.
static std::vector<std::vector<std::pair<int, int>>> odd_offset  =
{ { {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0} }, // offset for even rows
    { {0, 1}, {-1, 1}, {-1, 0}, {0, -1}, {1, 0}, {1, 1} }  // offset for odd rows
};

// This ugly fella returns a container of nodes which are neghbors of node n.
// We use the offset technique to determine the surrounding neighbor node.  Of course
// the neighbor nodes must be of the same color.

// For node n, it's neighbors are as follows:
//   2   1
// 3   n   0
//   4   5
std::vector<int> Board::getNeighbor(int n, Color c)
{
    std::vector<int> neighbor;
    Coord p = get_point(n);
    int row = std::get<0>(p);
    int col = std::get<1>(p);
    int offset = row % 2;
    int right = get_node(Coord{ row + odd_offset[offset][0].first, col + odd_offset[offset][0].second });
    int u_right = get_node(Coord{ row + odd_offset[offset][1].first, col + odd_offset[offset][1].second });
    int u_left = get_node(Coord{ row + odd_offset[offset][2].first, col + odd_offset[offset][2].second });
    int left = get_node(Coord{ row + odd_offset[offset][3].first, col + odd_offset[offset][3].second });
    int l_left = get_node(Coord{ row + odd_offset[offset][4].first, col + odd_offset[offset][4].second });
    int l_right = get_node(Coord{ row + odd_offset[offset][5].first, col + odd_offset[offset][5].second });
    if (isCorner(p))
    {
        if (isTopLeftCorner(p))
        {
            if (m_nodes[right] == c)
            {
                neighbor.push_back(right);
            }
            if (m_nodes[l_right] == c)
            {
                neighbor.push_back(l_right);
            }
        }
        if (isTopRightCorner(p))
        {
            if (m_nodes[left] == c)
            {
                neighbor.push_back(left);
            }
            if (m_nodes[l_left] == c)
            {
                neighbor.push_back(l_left);
            }
            if (m_nodes[l_right] == c)
            {
                neighbor.push_back(l_right);
            }
        }
        if (isBotLeftCorner(p))
        {
            if (m_nodes[right] == c)
            {
                neighbor.push_back(right);
            }
            if (m_nodes[u_right] == c)
            {
                neighbor.push_back(u_right);
            }
            if (m_nodes[u_left] == c)
            {
                neighbor.push_back(u_left);
            }
        }
        if (isBotRightCorner(p))
        {
            if (m_nodes[left] == c)
            {
                neighbor.push_back(left);
            }
            if (m_nodes[u_left] == c)
            {
                neighbor.push_back(u_left);
            }
        }
        return neighbor;
    }
    if (isInner(p))
    {
        // Inner nodes have 6 neighbors
        if (m_nodes[right] == c)
        {
            neighbor.push_back(right);
        }
        if (m_nodes[u_right] == c)
        {
            neighbor.push_back(u_right);
        }
        if (m_nodes[u_left] == c)
        {
            neighbor.push_back(u_left);
        }
        if (m_nodes[left] == c)
        {
            neighbor.push_back(left);
        }
        if (m_nodes[l_left] == c)
        {
            neighbor.push_back(l_left);
        }
        if (m_nodes[l_right] == c)
        {
            neighbor.push_back(l_right);
        }
        return neighbor;
    }
    if (isTop(p) || isBottom(p))
    {
        // Top/Bottom row nodes have 4 neighbors
        if (m_nodes[right] == c)
        {
            neighbor.push_back(right);
        }
        if (m_nodes[left] == c)
        {
            neighbor.push_back(left);
        }
        if (isTop(p))
        {
            if (m_nodes[l_left] == c)
            {
                neighbor.push_back(l_left);
            }
            if (m_nodes[l_right] == c)
            {
                neighbor.push_back(l_right);
            }
        }
        if (isBottom(p))
        {
            if (m_nodes[u_right] == c)
            {
                neighbor.push_back(u_right);
            }
            if (m_nodes[u_left] == c)
            {
                neighbor.push_back(u_left);
            }
        }
        return neighbor;
    }
    // Left side of the board
    if(isLHS(p))
    {
        if (m_nodes[right] == c)
        {
            neighbor.push_back(right);
        }
        if (m_nodes[u_right] == c)
        {
            neighbor.push_back(u_right);
        }
        if (m_nodes[l_right] == c)
        {
            neighbor.push_back(l_right);
        }
        if (1 == (row % 2))
        {
            if (m_nodes[u_left] == c)
            {
                neighbor.push_back(u_left);
            }
            if (m_nodes[l_left] == c)
            {
                neighbor.push_back(l_left);
            }
        }
        return neighbor;
    }
    // Right side of the board
    if(isRHS(p))
    {
        if (m_nodes[left] == c)
        {
            neighbor.push_back(left);
        }
        if (m_nodes[u_left] == c)
        {
            neighbor.push_back(u_left);
        }
        if (m_nodes[l_left] == c)
        {
            neighbor.push_back(l_left);
        }
        if (0 == (row % 2))
        {
            if (m_nodes[u_right] == c)
            {
                neighbor.push_back(u_right);
            }
            if (m_nodes[l_right] == c)
            {
                neighbor.push_back(l_right);
            }
        }
    }
    return neighbor;
}


// Overload << to print the ascii hex board
std::ostream& operator<<(std::ostream& out, const Board& b)
{
	int size = b.m_size;
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
			int square = b.get_node(Coord(i, j));
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


// For now we are just getting a random move, as long
// as it is valid.  Here is where the AI will happend for the
// next assignment.
Coord HexGame::getAIMove()
{
	Coord p{ INT_MAX, INT_MAX };
    Random r;
    bool bValid{ false };
    Timer t;

    while (!bValid)
    {
        int node = r.getRandom(0, m_size*m_size);
        p = m_board.get_point(node);
        bValid = m_board.isAvailable(p);
    }
    std::cout << "Took " << t.elapsed() << " seconds for the AI move." << std::endl;
    return p;
}


// Gets the user's move
Coord HexGame::getUserMove()
{
	Coord p{ INT_MAX, INT_MAX };
	std::string input = "";

	while (true)
	{
 
		std::cout << "Please enter the move co-ordinates (row col): ";
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
        if (m_board.isAvailable(Coord{ x, y }))
		{
            std::get<0>(p) = x;
            std::get<1>(p) = y;
			break;
		}
		std::cout << "Coordinates are either out of bounds or the square is already occupied." << std::endl;
	}
	return p;
}

// For now the src and dst containers represent the start and the end 
// for a specific player.  For the East->West player we find an occupied
// node on the East side (LHS) and see if there is a path to the West (RHS) of the board.
// For the North->South player we check for a path from the top to the bottom.
// In both cases we need make sure that the color is also correct.
bool HexGame::checkForPath(std::vector<int> src, std::vector<int> dst, Graph g, Color c)
{
    bool bResult{ false };

    for (auto s : src)
    {
        if (c == m_board.getColor(s))
        {
            for (auto d : dst)
            {
                if (c == m_board.getColor(d))
                {
                    bResult = g.path(s, d);
                    if (bResult)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return bResult;
}


// Main play loop
// For now we let the human player go first and
// the computer go second.  Next iteration we should allow for
// random start order and also allow for pie rule.
void HexGame::play()
{
    //m_board.generate();
    std::cout << "Start position:" << std::endl;
    std::cout << m_board;
    bool bWinner{ false };
    Graph first_player_graph = m_first_player.graph();
    Graph second_player_graph = m_second_player.graph();
	while (!bWinner)
	{
		Coord p = getUserMove();
		if (p != Coord{ INT_MAX, INT_MAX })
		{
            m_board.setColor(m_board.get_node(p), Color::COLOR_BLUE);
			std::cout << m_board;
            // We need to check if we have neighbors with same color nodes
            std::vector<int> firstneighbor = m_board.getNeighbor(m_board.get_node(p), Color::COLOR_BLUE);
            std::cout << "First player last move neighbors: " << firstneighbor << std::endl;
            for (auto n : firstneighbor)
            {
                first_player_graph.addEdge(m_board.get_node(p), n);
            }
            bWinner = checkForPath(m_board.east(), m_board.west(), first_player_graph, Color::COLOR_BLUE);
            if (bWinner)
            {
                std::cout << "Player one has won the game.";
                break;
            }
            // Let's get the computer move
            Coord m = getAIMove();
            m_board.setColor(m_board.get_node(m), Color::COLOR_RED);
            std::cout << "Computer moves: " << m << std::endl;
			std::cout << m_board;
            std::vector<int> secondneighbor = m_board.getNeighbor(m_board.get_node(m), Color::COLOR_RED);
            std::cout << "Second player last move neighbors: " << secondneighbor << std::endl;
            for (auto n : secondneighbor)
            {
                second_player_graph.addEdge(m_board.get_node(p), n);
            }
            bWinner = checkForPath(m_board.north(), m_board.south(), second_player_graph, Color::COLOR_RED);
            if (bWinner)
            {
                std::cout << "Computer has won the game.";
            }
		}
		else
		{
			// Attempting to quit the game.
			break;
		}
	}
}

// Main entry
int main()
{
	
	HexGame h{ 7 };

	h.play();
    return 0;
}

/*
Output from a 7x7 board:
Start position:
. - . - . - . - . - . - .
 \ / \ / \ / \ / \ / \ / \
  . - . - . - . - . - . - .
   \ / \ / \ / \ / \ / \ / \
    . - . - . - . - . - . - .
     \ / \ / \ / \ / \ / \ / \
      . - . - . - . - . - . - .
       \ / \ / \ / \ / \ / \ / \
        . - . - . - . - . - . - .
         \ / \ / \ / \ / \ / \ / \
          . - . - . - . - . - . - .
           \ / \ / \ / \ / \ / \ / \
            . - . - . - . - . - . - .
Please enter the move co-ordinates (row col): 3 3
. - . - . - . - . - . - .
 \ / \ / \ / \ / \ / \ / \
  . - . - . - . - . - . - .
   \ / \ / \ / \ / \ / \ / \
    . - . - . - . - . - . - .
     \ / \ / \ / \ / \ / \ / \
      . - . - . - B - . - . - .
       \ / \ / \ / \ / \ / \ / \
        . - . - . - . - . - . - .
         \ / \ / \ / \ / \ / \ / \
          . - . - . - . - . - . - .
           \ / \ / \ / \ / \ / \ / \
            . - . - . - . - . - . - .
First player last move neighbors: [ ]
Took 1.5316e-05 seconds for the AI move.
Computer moves: (3, 1)
. - . - . - . - . - . - .
 \ / \ / \ / \ / \ / \ / \
  . - . - . - . - . - . - .
   \ / \ / \ / \ / \ / \ / \
    . - . - . - . - . - . - .
     \ / \ / \ / \ / \ / \ / \
      . - R - . - B - . - . - .
       \ / \ / \ / \ / \ / \ / \
        . - . - . - . - . - . - .
         \ / \ / \ / \ / \ / \ / \
          . - . - . - . - . - . - .
           \ / \ / \ / \ / \ / \ / \
            . - . - . - . - . - . - .

<snip turns>


Took 1.0575e-05 seconds for the AI move.
Computer moves: (2, 0)
 . - . - . - . - . - . - .
  \ / \ / \ / \ / \ / \ / \
   . - . - . - R - . - . - .
    \ / \ / \ / \ / \ / \ / \
     R - . - . - . - . - R - .
      \ / \ / \ / \ / \ / \ / \
       . - R - . - B - R - B - .
        \ / \ / \ / \ / \ / \ / \
         . - B - B - B - B - B - B
          \ / \ / \ / \ / \ / \ / \
           . - . - R - . - . - R - .
            \ / \ / \ / \ / \ / \ / \
             . - . - . - R - . - . - .
Second player last move neighbors: [ ]
Please enter the move co-ordinates (row col): 4 0
 . - . - . - . - . - . - .
  \ / \ / \ / \ / \ / \ / \
   . - . - . - R - . - . - .
    \ / \ / \ / \ / \ / \ / \
     R - . - . - . - . - R - .
      \ / \ / \ / \ / \ / \ / \
       . - R - . - B - R - B - .
        \ / \ / \ / \ / \ / \ / \
         B - B - B - B - B - B - B
          \ / \ / \ / \ / \ / \ / \
           . - . - R - . - . - R - .
            \ / \ / \ / \ / \ / \ / \
             . - . - . - R - . - . - .
First player last move neighbors: [ 29 ]
Player one has won the game.
*/
