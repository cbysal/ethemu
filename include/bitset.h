#pragma once

#include <cstring>

class BitSet {
private:
  bool *data;

public:
  ~BitSet() { delete data; }

  void resize(int bitNum) {
    data = new bool[bitNum];
    memset(data, 0, bitNum * sizeof(bool));
  }

  void set(uint64_t pos) { data[pos] = true; }

  bool operator[](uint64_t pos) { return data[pos]; }
};