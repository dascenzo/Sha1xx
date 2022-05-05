#include "Algorithm.h"
#include <utility>

// 6.1 Method 1

static void Algorithm_a(Context& context, const Block& M_i) {
  for (auto it = M_i.begin(); it != M_i.end(); it += 4) {
    context.W[ (it - M_i.begin()) / 4 ] =
      *(it) << 24 | *(it+1) << 16 | *(it+2) << 8 | *(it+3);
  }
}
template<int t>
static auto Algorithm_b(std::array<Word, 80>& W) {
  static_assert(0 <= t && t <= 79);
  if constexpr (16 <= t && t <= 79) {
    W[t] = S_n<1>(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
  }
  return W[t];
}
static void Algorithm_c(Context& context) {
  context.A = context.H[0];
  context.B = context.H[1];
  context.C = context.H[2];
  context.D = context.H[3];
  context.E = context.H[4];
}
template<int t>
static void Algorithm_d_t(Context& context) {
  static_assert(0 <= t && t <= 79);
  Word TEMP = S_n<5>(context.A)
	    + f<t>(context.B, context.C, context.D)
	    + context.E
            + Algorithm_b<t>(context.W)
	    + K<t>();
  context.E = context.D;
  context.D = context.C;
  context.C = S_n<30>(context.B);
  context.B = context.A;
  context.A = TEMP;
}
template<std::size_t ... t>
static void Algorithm_d(Context& context, std::index_sequence<t...> sequence) {
  static_assert(sequence.size() == 80);
  (Algorithm_d_t<t>(context), ...);
}
static void Algorithm_d(Context& context) {
  Algorithm_d(context, std::make_index_sequence<80>());
}
static void Algorithm_e(Context& context) {
  context.H[0] += context.A;
  context.H[1] += context.B;
  context.H[2] += context.C;
  context.H[3] += context.D;
  context.H[4] += context.E;
}
void Algorithm::initContext(Context& context) {
  context.H[0] = 0x67452301;
  context.H[1] = 0xEFCDAB89;
  context.H[2] = 0x98BADCFE;
  context.H[3] = 0x10325476;
  context.H[4] = 0xC3D2E1F0;
}
void Algorithm::processMessageBlock(Context& context, const Block& M_i) {
  Algorithm_a(context, M_i);
  //Algorithm_b // executed as part of Algorithm_d
  Algorithm_c(context);
  Algorithm_d(context);
  Algorithm_e(context);
}
