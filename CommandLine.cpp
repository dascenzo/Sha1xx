#include "CommandLine.h"
#include <cstring>

static bool isEqual(const char* s1, const char* s2) {
  return std::strcmp(s1, s2) == 0;
}
UnknownCommandLine::UnknownCommandLine()
  : std::runtime_error{"unknown command line"} {
}
// $ prog -h [ anything ] => show help
// $ prog <file>          => read from file
// $ prog -- <file>       => read from file (that may start with "-")
CommandLine parseCommandLine(int argc, char* argv[]) {
  if (argc >= 2 && isEqual(argv[1], "-h")) {
    return ShowHelp{};
  }
  else if (argc == 2 && argv[1][0] != '-') {
    ReadFromFile fromFile{};
    fromFile.fileName = argv[1];
    return fromFile;
  }
  else if (argc == 3 && isEqual(argv[1], "--")) {
    ReadFromFile fromFile{};
    fromFile.fileName = argv[2];
    return fromFile;
  }
  throw UnknownCommandLine{};
}
