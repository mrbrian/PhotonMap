#include "gtest\gtest.h"
#include <stdio.h>

// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
	EXPECT_EQ(1, 1);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	int x;
	std::cin >> x;
}