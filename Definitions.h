#pragma once
#include <cstdint>
#include <array>
#include <climits>

#define HASH_BYTES 20

// A block is 512 bits: (512 bits / 8 = 2^9 / 2^3 = 2^6 = 64 bytes)
#define BLOCK_BYTES 64

// Message length in bits is held by an 8 byte value.
#define LENGTH_BYTES 8

// A word equals a 32-bit string
using Word = uint32_t;

// Types to store a SHA-1 hash in words and bytes
using ByteHash = std::array<uint8_t, HASH_BYTES>;
using WordHash = std::array<Word, HASH_BYTES / 4>; 

// SHA-1 sequentially processes the data in blocks when computing the message
// digest.
using Block = std::array<uint8_t, BLOCK_BYTES>;

// 8 byte value holding the length of the message in bits
using MessageLengthType = uint64_t;

// Holds variables defined in RFC3174
struct Context {
  Word A, B, C, D, E;
  WordHash H;
  std::array<Word, 80> W;
};
// 3.c circular left shift operation
template<int n>
Word S_n(Word X) {
  static_assert(0 <= n && n < 32);
  return (X << n) | (X >> (32-n));
}
// 5. Functions f(0), f(1), ... , f(79)
template<int t>
Word f(Word B, Word C, Word D) {
  static_assert(0 <= t && t <= 79);
  if constexpr (0 <= t && t <= 19) {
    return (B & C) | (~B & D);
  }
  else if (20 <= t && t <= 39) {
    return B ^ C ^ D;
  }
  else if (40 <= t && t <= 59) {
    return (B & C) | (B & D) | (C & D);
  }
  else /* 60 <= t && t <= 79 */ {
    return B ^ C ^ D;
  }
} 
// 5. Constants K(0), K(1), ... , K(79)
template<int t>
Word K() {
  static_assert(0 <= t && t <= 79);
  if constexpr (0 <= t && t <= 19) return 0x5A827999;
  else if (20 <= t && t <= 39) return 0x6ED9EBA1;
  else if (40 <= t && t <= 59) return 0x8F1BBCDC;
  else /*60 <= t && t <= 79 */ return 0xCA62C1D6;
}
