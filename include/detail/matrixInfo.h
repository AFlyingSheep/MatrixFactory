#ifndef _MATRIX_FACTOR_DETAIL_MATRIX_INFO_H_
#define _MATRIX_FACTOR_DETAIL_MATRIX_INFO_H_

struct MatrixMarketHeader {
  std::string object;
  std::string format;
  std::string field;
  std::string symmetry;
  int rows;
  int cols;
  int nonzeros;
};

struct MatrixConfig {
  // We don't check whether the matrix is symmetric or not, so when user
  // set this flag, we will convert the matrix to a lower triangular matrix but
  // maybe it is not safe.
  bool is_convert_to_triangular_matrix = false;

  // When the matrix is a pattern matrix, we need to set the padding value,
  // but user can change the matrix value later if they want.
  double padding_value = 0.0;

  // When the matrix is symmetric, maybe the source matrix's indices is not
  // sorted, so if the flag is true, we will sort the indices.
  // Note: the sort will be performed during the matrix converted into CSR or
  // CSC.
  bool sort_indices = false;
};

#endif
