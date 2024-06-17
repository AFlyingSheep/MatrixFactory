#include "matrixConverter.h"
#include "matrixMarketReader.h"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

int main() {
  std::string filename =
      "/staff/zhaoyang/project/MatrixFactory/data/example.mtx";
  auto mm = readMatrixMarketFile<double>(filename);
  mm.info(true);
  printf("Matrix reading over.\n");
  auto csr = convertToCSR(mm);
  csr.info(true);
}
