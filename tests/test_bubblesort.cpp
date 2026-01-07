#include "utils.h"
#include <gtest/gtest.h>

class BubbleSortTest : public ::testing::Test {
protected:
  void SetUp() override {}
};

TEST_F(BubbleSortTest, SortArray) {
  std::string source = R"(
    fn bubbleSort(arr, n) {
      let i = 0;
      while (i < n - 1) {
        let j = 0;
        while (j < n - i - 1) {
          if (arr[j] > arr[j + 1]) {
            let temp = arr[j];
            arr[j] = arr[j + 1];
            arr[j + 1] = temp;
          }
          j = j + 1;
        }
        i = i + 1;
      }
    }

    let arr = [64, 34, 25, 12, 22, 11, 90];
    bubbleSort(arr, 7);
    
    let k = 0;
    while (k < 7) {
      print(arr[k]);
      k = k + 1;
    }
  )";

  std::string output = TestUtils::compileAndRun(source);

  // Expected sorted output: 11, 12, 22, 25, 34, 64, 90
  std::string expected = "11\n12\n22\n25\n34\n64\n90\n";
  EXPECT_EQ(output, expected);
}
