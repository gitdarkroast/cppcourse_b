#include "pch.h"

#include "../hex/hex.h"

namespace {
	class BoardTest : public ::testing::Test
	{
	};

	TEST(BoardTest, TestDefaultSize) {
		EXPECT_TRUE(new Board());
	}
	TEST(BoardTest, TestSizeProvided) {
		int size{ 10 };
		EXPECT_TRUE(new Board(size));
	}
	TEST(BoardTest, TestGetNodeValue) {
		int size{ 10 };
		Board b(size);

		EXPECT_EQ(0, b.get_node(Point(0, 0)));
		EXPECT_EQ(size*size, b.get_node(Point(size, 0)));
		EXPECT_EQ(size, b.get_node(Point(0, size)));
		EXPECT_EQ(size*size + size, b.get_node(Point(size, size)));
	}

	TEST(BoardTest, TestIsCorner) {
		int size{ 10 };
		Board b(size);

		EXPECT_TRUE(b.isCorner(Point(0, 0)));
		EXPECT_FALSE(b.isCorner(Point(0, 9)));
		EXPECT_FALSE(b.isCorner(Point(9, 0)));
		EXPECT_FALSE(b.isCorner(Point(9, 9)));
	}

	TEST(BoardTest, TestNotCorner) {
		int size{ 10 };
		Board b(size);

		EXPECT_FALSE(b.isCorner(Point(1, 1)));
		EXPECT_FALSE(b.isCorner(Point(0, 10)));
		EXPECT_FALSE(b.isCorner(Point(5, 0)));
	}

	TEST(BoardTest, TestIsOccupied) {
		int size{ 10 };
		Board b(size);

		EXPECT_FALSE(b.isOccupied(0));
	}

	TEST(BoardTest, TestIsWithinBoundary) {
		int size{ 10 };
		Board b(size);

		EXPECT_TRUE(b.isWithinBoundary(Point(0, 0)));
		EXPECT_TRUE(b.isWithinBoundary(Point(size-1, size-1)));
		EXPECT_TRUE(b.isWithinBoundary(Point(0, size-1)));
		EXPECT_TRUE(b.isWithinBoundary(Point(size-1, 0)));
		EXPECT_TRUE(b.isWithinBoundary(Point(5, 0)));
	}

	TEST(BoardTest, TestNotIsWithinBoundary) {
		int size{ 10 };
		Board b(size);

		EXPECT_FALSE(b.isWithinBoundary(Point(0, size)));
		EXPECT_FALSE(b.isWithinBoundary(Point(size, size)));
		EXPECT_FALSE(b.isWithinBoundary(Point(0, size)));
		EXPECT_FALSE(b.isWithinBoundary(Point(size, 0)));
		EXPECT_FALSE(b.isWithinBoundary(Point(-1, size+1)));
	}

	TEST(BoardTest, TestIsAvailable) {
		int size{ 10 };
		Board b(size);

		EXPECT_TRUE(b.isAvailable(Point(0, 0)));
		EXPECT_TRUE(b.isAvailable(Point(5, 0)));
	}

	TEST(BoardTest, TestNotIsAvailable) {
		int size{ 10 };
		Board b(size);

		b.makeMove(Point(0, 0), Color::COLOR_BLUE);
		EXPECT_FALSE(b.isAvailable(Point(0, 0)));
		b.makeMove(Point(5, 0), Color::COLOR_RED);
		EXPECT_FALSE(b.isAvailable(Point(5, 0)));
	}
} // namespace

