#include "Sha1.h"
#include "Definitions.h"
#include "Algorithm.h"
#include "BlockStream.h"
#include <cstdio>
#include <array>
#include <utility>
#include <string>
#include <cassert>

// converts a hash in the form of words to bytes
static void words2Bytes(const WordHash& wordHash, ByteHash& byteHash) {
  auto byteIt = byteHash.begin(); 
  for (auto word : wordHash) {
    *byteIt++ = (word & 0xff000000) >> 24;
    *byteIt++ = (word & 0x00ff0000) >> 16;
    *byteIt++ = (word & 0x0000ff00) >> 8;
    *byteIt++ = (word & 0x000000ff) >> 0;
  }
}

// Behaves similar to UnformattedInputFunction, but doesn't set gcount, or
// catch exceptions. If successful, stream will be ONLY eof.
std::istream& operator>>(std::istream& in, Sha1& sha1) {
  if (in.eof() || in.bad()) {
    in.setstate(std::ios_base::failbit);
  }
  if (in.good()) {
    Context context;
    bool newBlock; // a new block was produced (written to input arg) from block stream
    BlockStream blockStream;
    Block M_i;
    Algorithm::initContext(context);
    while (in.good()) {
      in.peek(); // three possibilities after peek():
      // * eof: hit the end, all characters have been read, fail & bad not set
      // * good(): there's a char available, not at the end yet, no error
      // * error: bad, fail(?)
      if (in) {
        if (in.eof()) {
          while ( (newBlock = blockStream.done(M_i)) ) {
            Algorithm::processMessageBlock(context, M_i);
          }
        }
        else {
          char const c{ std::istream::traits_type::to_char_type(in.get()) }; 
          if ( (newBlock = blockStream.next(c, M_i)) ) {
            Algorithm::processMessageBlock(context, M_i);
          }
        }
      } // else error
    }
    if (in) {
      assert(in.eof());
      words2Bytes(context.H, sha1.m_hash);
    }
  }
  return in;
}
std::string Sha1::toString() const {
  const auto digits = "0123456789abcdef";
  auto string = std::array<char, HASH_BYTES * 2>();
  auto c = string.begin(); 
  for (auto byte : m_hash) {
    *c++ = digits[ (byte & 0xf0) >> 4 ]; 
    *c++ = digits[ (byte & 0x0f) ];
  }
  return {string.data(), string.size()};
}
bool Sha1::operator==(const Sha1& other) const {
  return m_hash == other.m_hash;
}
bool Sha1::operator!=(const Sha1& other) const {
  return !(*this == other);
}
static void assertIndexInRange(int idx) {
  if (idx < 0 || idx >= HASH_BYTES) {
    throw std::out_of_range("invalid index: " + std::to_string(idx));
  }
}
uint8_t Sha1::operator[](int idx) const {
  assertIndexInRange(idx);
  return m_hash[idx];
}
uint8_t& Sha1::operator[](int idx) {
  assertIndexInRange(idx);
  return m_hash[idx];
}
