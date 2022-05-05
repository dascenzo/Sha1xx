#include "BlockStream.h"
#include <algorithm>
#include <cassert>

BlockStream::BlockStream()
  :m_messageLength(0), m_bufpos(0), m_state(ReadingBlocks) {
}
bool BlockStream::done(Block& M) {
  switch (m_state) {
  case ReadingBlocks:
    {
      pad();
      m_state = (m_bufpos == BLOCK_BYTES ? Halt : ExtraPadBlock);
      std::copy(m_buf.begin(), m_buf.begin() + BLOCK_BYTES, M.begin()); 
      return true;
    }
  case ExtraPadBlock:
    {
      m_state = Halt;
      std::copy(m_buf.begin() + BLOCK_BYTES, m_buf.end(), M.begin()); 
      return true;
    }
  case Halt:
    break;
  }
  return false;
}
bool BlockStream::next(char c, Block& M) {
  if (isDone()) {
    throw std::logic_error{"can't add data: stream already done"};
  }
  m_buf[m_bufpos++] = c;
  m_messageLength += CHAR_BIT; // bits increase
  if (m_bufpos == BLOCK_BYTES) {
    m_bufpos = 0;
    std::copy(m_buf.begin(), m_buf.begin() + BLOCK_BYTES, M.begin()); 
    return true;
  }
  return false;
}
// 4. Message Padding
// subroutine
// preconditions: m_state == ReadingBlocks && m_bufpos < BLOCK_BYTES
// postconditions: m_buf is padded && m_bufpos == BLOCK_BYTES || BLOCK_BYTES*2
void BlockStream::pad() {
  assert(m_state == ReadingBlocks);
  assert(m_bufpos < BLOCK_BYTES);
  // Append bit "1". Pad with the minimum number of "0"s (seven, i.e. 10000000b) 
  // before the length bytes, which must start at least at the next byte since
  // the length occupies a fixed number of bytes at the end.
  m_buf[m_bufpos] = 0x80;
  m_bufpos += 1;

  const int zerosCount = ((BLOCK_BYTES - LENGTH_BYTES - m_bufpos >= 0)
      ? BLOCK_BYTES : BLOCK_BYTES * 2) - LENGTH_BYTES - m_bufpos;

  std::fill_n(&m_buf[m_bufpos], zerosCount, 0);
  m_bufpos += zerosCount;

  for (int i = 0; i < LENGTH_BYTES; ++i) {
    const int shift = (LENGTH_BYTES - 1 - i) * CHAR_BIT; 
    m_buf[m_bufpos++] = 0xff & (m_messageLength >> shift);
  }
  assert(m_bufpos == BLOCK_BYTES || m_bufpos == BLOCK_BYTES*2);
}
bool BlockStream::isDone() const {
  switch (m_state) {
  case ReadingBlocks: return false;
  case ExtraPadBlock:
  case Halt: return true;
  }
}
bool BlockStream::isExhausted() const {
  switch (m_state) {
  case ReadingBlocks:
  case ExtraPadBlock: return false;
  case Halt: return true;
  }
}
