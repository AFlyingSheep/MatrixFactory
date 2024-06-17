
#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "detail/complex.h"
#include "detail/error.h"
#include "detail/matrixInfo.h"
#include "matrixFormat.h"

// Read the corrdinate format matrix market file
template <typename T>
void readCorrdinate(MatrixMarket<T> &matrix, MatrixMarketHeader header,
                    std::ifstream &infile, MatrixConfig &config) {
  // Read the data
  int row, col;
  T value;
  int dig_count = 0;

  while (infile >> row >> col) {
    // Only read the lower triangular matrix
    if (config.is_convert_to_triangular_matrix ||
        header.symmetry != "general") {
      if (row < col) {
        if (header.field != "pattern") {
          infile >> value;
        }
        continue;
      }
    }

    // Read the values
    if (header.field == "pattern") {
      // Transform the 1~N to 0~N-1
      matrix.data.emplace_back(row - 1, col - 1, config.padding_value);
    } else if (header.field == "complex") {
      printf("Need to fix...");
    } else {
      infile >> value;
      matrix.data.emplace_back(row - 1, col - 1, value);
    }
  }
}

// Read the matrix market file, *.mtx
template <typename T>
MatrixMarket<T> readMatrixMarketFile(const std::string &filename,
                                     MatrixConfig config = MatrixConfig()) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    throw std::runtime_error("Can't open the file\n" + filename);
  }

  MatrixMarket<T> mm;
  std::string line;
  MatrixMarketHeader header;

  // Read the header
  std::getline(infile, line);
  if (line.substr(0, 14) != std::string("%%MatrixMarket")) {
    std::cout << line.substr(0, 14) << std::endl;
    std::cout << std::string("%%MatrixMarket") << std::endl;
    throw std::runtime_error("Not a valid MatrixMarket file.\n");
  }

  std::istringstream header_in(line);
  std::string banner;
  header_in >> banner >> header.object >> header.format >> header.field >>
      header.symmetry;

  // Output header information for debug
  std::cout << "Object: " << header.object << std::endl;
  std::cout << "Format: " << header.format << std::endl;
  std::cout << "Field: " << header.field << std::endl;
  std::cout << "Symmetry: " << header.symmetry << std::endl;

  // Ignore the annotation
  while (std::getline(infile, line)) {
    if (line[0] != '%') {
      break;
    }
  }

  if (header.format == "coordinate") {
    // Read the matrix dim and the count of non-zero elements
    // (For coord format)
    std::istringstream sizeLine(line);
    sizeLine >> mm.rows >> mm.cols >> mm.nonzeros;

    // Read the matrix data
    readCorrdinate(mm, header, infile, config);
  } else {
    // For array, maybe coming soon...
    throw std::runtime_error("The format is not supported.\n");
  }

  infile.close();
  return mm;
}
