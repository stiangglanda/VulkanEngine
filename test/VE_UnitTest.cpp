#include <Core.h>
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(CoreTest, HelloWorldExpectTrue)
{
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(Core::PrintHelloWorld(), true);
}
