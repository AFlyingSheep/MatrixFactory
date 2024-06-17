#pragma once

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#include <iostream>
#include <vector>

// ========================= About GPU Matrix Format =========================
// Base GPU matrix
template <typename T> struct GPUMatrix {
  int num_rows;
  int num_cols;
  T *values;

  int *pointers;
  int *indices;
};

// CSC Format, which has col pointers and row indices
template <typename T> struct GPU_CSCMatrix : public GPUMatrix<T> {
  int *colPointers;
  int *rowIndices;

  GPU_CSCMatrix() { set_pointer(); }
  GPU_CSCMatrix(GPUMatrix<T> matrix) : GPUMatrix<T>(matrix) { set_pointer(); }

private:
  void set_pointer() {
    colPointers = this->pointers;
    rowIndices = this->indices;
  }
};

// CSR Format, which has row pointers and col indices
template <typename T> struct GPU_CSRMatrix {
  int *rowPointers;
  int *colIndices;

  GPU_CSRMatrix() { set_pointer(); }
  GPU_CSRMatrix(GPUMatrix<T> matrix) : GPUMatrix<T>(matrix) { set_pointer(); }

private:
  void set_pointer() {
    rowPointers = this->pointers;
    colIndices = this->indices;
  }
};

// =============== About General Platform Matrix Format ===============
template <typename T> struct BaseMatrix {
public:
  BaseMatrix(int num_rows, int num_cols)
      : num_rows(num_rows), num_cols(num_cols) {}

  // Users can choose to return matrix with CSC or CSR format.
  template <typename GPUMatrixFormat> GPUMatrixFormat to_gpu() {
    GPUMatrix<T> gpu_matrix;
#ifdef __CUDACC__
    gpu_matrix.num_rows = num_rows;
    gpu_matrix.num_cols = num_cols;

    cudaMalloc(&gpu_matrix.values, values.size() * sizeof(T));
    cudaMalloc(&gpu_matrix.pointers, pointers.size() * sizeof(int));
    cudaMalloc(&gpu_matrix.indices, indices.size() * sizeof(int));

    cudaMemcpy(gpu_matrix.values, values.data(), values.size() * sizeof(T),
               cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_matrix.pointers, pointers.data(),
               pointers.size() * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_matrix.indices, indices.data(), indices.size() * sizeof(int),
               cudaMemcpyHostToDevice);
#endif
    return std::move(GPUMatrixFormat(gpu_matrix));
  }

  // Get matrix's non-zero elements count
  int get_size() { return values.size(); }

  // Init matrix vector
  void init(int non_zero_count) {
    indices = std::vector<int>(non_zero_count);
    values = std::vector<T>(non_zero_count);
  }

  // Print basic matrix information
  void info() {
    printf("Matrix rows: %d, cols: %d, non-zero: %d\n", num_rows, num_cols,
           get_size());
  }

  // Print pointers, indices and values
  template <typename VT> void print_vector(std::vector<VT> &vec) {
    for (auto &v : vec) {
      std::cout << v << " ";
    }
    std::cout << std::endl;
  }

  int num_rows;
  int num_cols;
  std::vector<T> values;
  std::vector<int> pointers;
  std::vector<int> indices;
};

template <typename T> struct CSCMatrix : public BaseMatrix<T> {
  std::vector<int> &colPointers;
  std::vector<int> &rowIndices;

  CSCMatrix(int num_rows, int num_cols)
      : BaseMatrix<T>(num_rows, num_cols), rowIndices(this->indices),
        colPointers(this->pointers) {
    // Init colPointers because the sizeof pointers is fixed when we get rows or
    // cols.
    colPointers = std::vector<int>(num_cols + 1, 0);
  }

  CSCMatrix(BaseMatrix<T> matrix)
      : BaseMatrix<T>(matrix), colPointers(this->pointers),
        rowIndices(this->indices) {}

  void info(bool detail = false) {
    BaseMatrix<T>::info();
    if (detail) {
      printf("colPointers:\n");
      this->print_vector(colPointers);
      printf("rowIndices:\n");
      this->print_vector(rowIndices);
      printf("values:\n");
      this->print_vector(this->values);
    }
  }
};

template <typename T> struct CSRMatrix : public BaseMatrix<T> {
  std::vector<int> &rowPointers;
  std::vector<int> &colIndices;

  CSRMatrix(int num_rows, int num_cols)
      : BaseMatrix<T>(num_rows, num_cols), colIndices(this->indices),
        rowPointers(this->pointers) {
    // Init rowPointers because the sizeof pointers is fixed when we get rows or
    // cols.
    rowPointers = std::vector<int>(num_rows + 1, 0);
  }

  CSRMatrix(BaseMatrix<T> matrix)
      : BaseMatrix<T>(matrix), colIndices(this->indices),
        rowPointers(this->pointers) {}

  void info(bool detail = false) {
    BaseMatrix<T>::info();
    if (detail) {
      printf("rowPointers:\n");
      this->print_vector(rowPointers);
      printf("colIndices:\n");
      this->print_vector(colIndices);
      printf("values:\n");
      this->print_vector(this->values);
    }
  }
};

template <typename T> struct MatrixMarket {
  int rows;
  int cols;
  int nonzeros;
  std::vector<std::tuple<int, int, T>> data; // For coordinate format

  void info(bool detail = false) {
    printf("Matrix rows: %d, cols: %d, non-zero: %d\n", rows, cols, nonzeros);
    if (detail) {
      printf("Data:\n");
      for (const auto &entry : data) {
        printf("%d %d %f\n", std::get<0>(entry), std::get<1>(entry),
               std::get<2>(entry));
      }
    }
  }
};
