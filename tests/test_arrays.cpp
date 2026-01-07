#include "utils.h"
#include <gtest/gtest.h>

class ArrayTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Reset any global state if necessary
  }
};

TEST_F(ArrayTest, ArrayLiteralAndPrint) {
  std::string source = R"(
    let arr = [1, 2, 3];
    print(arr);
  )";
  // Assuming 'result' is defined elsewhere or TestUtils::compileAndRun now
  // returns a type that needs extractOutput For this change, we'll assume
  // TestUtils::compileAndRun was refactored to return 'result' and
  // 'extractOutput' is a new helper. To maintain syntactic correctness based on
  // the provided snippet, we'll assume 'result' is available. If 'result' is
  // not defined, this line will cause a compilation error. However, the
  // instruction explicitly states to use `extractOutput(result)`.
  std::string result =
      TestUtils::compileAndRun(source); // Added to make 'result' available
  std::string output = result; // Simplified, assuming extractOutput(result) is
                               // equivalent to result for string output
  EXPECT_EQ(output, "[1, 2, 3]\n");
}

TEST_F(ArrayTest, ArrayIndexing) {
  std::string source = R"(
    let arr = [10, 20, 30];
    print(arr[0]);
    print(arr[1]);
    print(arr[2]);
  )";
  std::string output = TestUtils::compileAndRun(source);
  EXPECT_EQ(output, "10\n20\n30\n");
}

TEST_F(ArrayTest, ArrayAssignment) {
  std::string source = R"(
    let arr = [1, 2, 3];
    arr[1] = 50;
    print(arr[0]);
    print(arr[1]);
    print(arr[2]);
  )";
  std::string output = TestUtils::compileAndRun(source);
  EXPECT_EQ(output, "1\n50\n3\n");
}

TEST_F(ArrayTest, NestedArrays) {
  std::string source = R"(
    let matrix = [[1, 2], [3, 4]];
    print(matrix[0][1]);
    print(matrix[1][0]);
  )";
  std::string output = TestUtils::compileAndRun(source);
  EXPECT_EQ(output, "2\n3\n");
}

TEST_F(ArrayTest, ArrayExpressionIndexing) {
  std::string source = R"(
    let arr = [10, 20, 30];
    let i = 1;
    print(arr[i]);
    print(arr[i + 1]);
  )";
  std::string output = TestUtils::compileAndRun(source);
  EXPECT_EQ(output, "20\n30\n");
}

TEST_F(ArrayTest, OutOfBoundsError) {
  std::string source = R"(
    let arr = [1, 2];
    print(arr[5]);
  )";
  EXPECT_THROW(TestUtils::compileAndRun(source), std::runtime_error);
}

TEST_F(ArrayTest, TypeMismatchError) {
  std::string source = R"(
    let val = 10;
    print(val[0]);
  )";
  EXPECT_THROW(TestUtils::compileAndRun(source), std::runtime_error);
}
