#ifndef __MATRIX_FACTORY_DETAIL_COMPLEX_H_
#define __MATRIX_FACTORY_DETAIL_COMPLEX_H_

// Define complex number structure
template <typename T> struct Complex {
  T real;
  T imag;

  Complex() : real(0), imag(0) {}
  Complex(T real, T imag) : real(real), imag(imag) {}
};

// Define complex number addition
template <typename T>
Complex<T> operator+(const Complex<T> &a, const Complex<T> &b) {
  return Complex<T>(a.real + b.real, a.imag + b.imag);
}

// Define complex number subtraction
template <typename T>
Complex<T> operator-(const Complex<T> &a, const Complex<T> &b) {
  return Complex<T>(a.real - b.real, a.imag - b.imag);
}

// Define complex number multiplication
template <typename T>
Complex<T> operator*(const Complex<T> &a, const Complex<T> &b) {
  return Complex<T>(a.real * b.real - a.imag * b.imag,
                    a.real * b.imag + a.imag * b.real);
}

// Define complex number division
template <typename T>
Complex<T> operator/(const Complex<T> &a, const Complex<T> &b) {
  T denominator = b.real * b.real + b.imag * b.imag;
  return Complex<T>((a.real * b.real + a.imag * b.imag) / denominator,
                    (a.imag * b.real - a.real * b.imag) / denominator);
}

// Define complex number equality
template <typename T>
bool operator==(const Complex<T> &a, const Complex<T> &b) {
  return a.real == b.real && a.imag == b.imag;
}

// Define complex number inequality
template <typename T>
bool operator!=(const Complex<T> &a, const Complex<T> &b) {
  return a.real != b.real || a.imag != b.imag;
}

// Define complex number output
template <typename T>
std::ostream &operator<<(std::ostream &os, const Complex<T> &a) {
  os << a.real << " + " << a.imag << "i";
  return os;
}

#endif
