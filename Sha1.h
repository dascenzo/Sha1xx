#pragma once
#include <string>
#include "Definitions.h"

class Sha1 {
public:
  // returns 40 character lowercase hex string representation
  std::string toString() const;

  // access byte of the hash, throws if idx is out of range
  uint8_t operator[](int idx) const;
  uint8_t& operator[](int idx);

  bool operator==(const Sha1&) const;
  bool operator!=(const Sha1&) const;

  // hash the all data from the stream; the Sha1 object is only updated if the
  // stream is able to be emptied withouth error (i.e. reach EOF).
  friend std::istream& operator>>(std::istream&, Sha1&);
private:
  ByteHash m_hash;
};
std::istream& operator>>(std::istream&, const Sha1&);
