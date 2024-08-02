#ifndef UTILS
#define UTILS

template<class T>
bool in_range(T i, T v, T e) {
  return i <= v && v <= e;
}

#define RAND_DOUBLE (double(rand()%10000)/10000.0)
#define round(X) static_cast<int>(floor(X+0.5))
#define square(X) (X) * (X)
#define cube(X) (X) * (X) * (X)

#endif
