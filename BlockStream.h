#pragma once
#include "Definitions.h"
#include <istream>

// 4. Message Padding
// This class takes a data stream and presents it as a sequence of blocks,
// where the last is padded if necessary.
class BlockStream {
public:
  BlockStream();

  // Add byte to stream, and if now a complete block has been added, fill M
  // with that data and return true. Throws exception if done() has previously
  // been called.
  bool next(char c, Block& M);

  // Mark stream done - no more bytes will be added. M will be filled with
  // remaining (and correctly padded) data. There may be multiple blocks
  // available, so continue calling until false is returned.
  bool done(Block& M);

  // Return true if done() was called. 
  bool isDone() const; 

  // Return true if no more blocks will be produced.
  bool isExhausted() const;
private:
  void pad();

  MessageLengthType m_messageLength;
  int m_bufpos;
  std::array<char, BLOCK_BYTES * 2> m_buf;
  enum {
    ReadingBlocks, ExtraPadBlock, Halt
  } m_state;
};
