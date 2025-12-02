#include <gtest/gtest.h>

class CompilerScaffoldTest : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {

    }
};


TEST_F(CompilerScaffoldTest, PlaceholderTest) {
    EXPECT_EQ(1, 1);
}

// ============================================================================
// Google Test Main Entry Point
// ============================================================================

// GTEST_MAIN is linked via gtest_main library in CMakeLists.txt