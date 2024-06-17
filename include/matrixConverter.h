#ifndef _MATRIX_FACTORY_DETAIL_MATRIX_CONVERTER_H_
#define _MATRIX_FACTORY_DETAIL_MATRIX_CONVERTER_H_

#include <tuple>

#include "detail/error.h"
#include "matrixFormat.h"

enum class MatrixType { COO, CSR, CSC, DENSE };

template <typename T, MatrixType Type>
BaseMatrix<T> convertToSparse(const MatrixMarket<T> &matrix) {
  BaseMatrix<T> base(matrix.rows, matrix.cols);
  base.init(matrix.nonzeros);

  std::vector<int> counts;

  // Compute the number of nonzeros in each row or column
  if constexpr (Type == MatrixType::CSC) {
    counts = std::vector<int>(matrix.cols, 0);
    base.pointers = std::vector<int>(matrix.cols + 1, 0);
  } else if constexpr (Type == MatrixType::CSR) {
    counts = std::vector<int>(matrix.rows, 0);
    base.pointers = std::vector<int>(matrix.rows + 1, 0);
  } else {
    // Throw exception
    throw std::runtime_error("In convert, matrix format is not supported");
  }

  for (const auto &entry : matrix.data) {
    int index =
        (Type == MatrixType::CSC) ? std::get<1>(entry) : std::get<0>(entry);
    counts[index]++;
  }

  // Fill the pointers
  base.pointers[0] = 0;
  int end = (Type == MatrixType::CSC) ? matrix.cols : matrix.rows;
  for (int index = 1; index <= end; ++index) {
    base.pointers[index] = base.pointers[index - 1] + counts[index - 1];
  }

  MATRIX_ASSERT(base.pointers[end] == matrix.nonzeros,
                "The number of nonzeros is not correct");

  // Fill the indices and values
  std::vector<int> currentPos(end, 0);
  for (int index = 0; index < end; ++index) {
    currentPos[index] = base.pointers[index];
  }

  for (const auto &entry : matrix.data) {
    int row = std::get<0>(entry);
    int col = std::get<1>(entry);
    T value = static_cast<T>(std::get<2>(entry));

    if constexpr (Type == MatrixType::CSC) {
      int pos = currentPos[col];
      base.indices[pos] = row;
      base.values[pos] = value;
      currentPos[col]++;
    } else if constexpr (Type == MatrixType::CSR) {
      int pos = currentPos[row];
      base.indices[pos] = col;
      base.values[pos] = value;
      currentPos[row]++;
    }
  }

  return std::move(base);
}

template <typename T> CSCMatrix<T> convertToCSC(const MatrixMarket<T> &matrix) {
  return CSCMatrix<T>(convertToSparse<T, MatrixType::CSC>(matrix));
}

template <typename T> CSRMatrix<T> convertToCSR(const MatrixMarket<T> &matrix) {
  return CSRMatrix<T>(convertToSparse<T, MatrixType::CSR>(matrix));
}

#endif
