#pragma once

#include <cstddef>

struct Hash {
  unsigned char data[32];
  bool operator==(const Hash &hash) const {
    for (int i = 0; i < 32; i++)
      if (data[i] != hash.data[i])
        return false;
    return true;
  }

  std::string toString() const {
    std::string str;
    str.resize(66);
    str[0] = '0';
    str[1] = 'x';
    for (int i = 0; i < 32; i++) {
      unsigned char h = (data[i] >> 4) & 0xf;
      unsigned char l = data[i] & 0xf;
      char hc = h < 10 ? h + '0' : h - 10 + 'a';
      char lc = l < 10 ? l + '0' : l - 10 + 'a';
      str[2 + i * 2] = hc;
      str[3 + i * 2] = lc;
    }
    return str;
  }

  void parse(const std::string &str) { std::copy(str.begin(), str.end(), data); }

  std::string bytes() const { return std::string(data, data + 32); }
};

struct HashHash {
  size_t operator()(const Hash &hash) const {
    return (size_t(hash.data[0]) << 56) | (size_t(hash.data[1]) << 48) | (size_t(hash.data[2]) << 40) |
           (size_t(hash.data[3]) << 32) | (size_t(hash.data[4]) << 24) | (size_t(hash.data[5]) << 16) |
           (size_t(hash.data[6]) << 8) | size_t(hash.data[7]);
  }
};
