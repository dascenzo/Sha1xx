#pragma once
#include <variant>
#include <stdexcept>

struct ReadFromFile {
  const char* fileName; // pointer will be == argv[x]
};
struct ShowHelp {
};
struct UnknownCommandLine : std::runtime_error {
  UnknownCommandLine();
};
using CommandLine =
  std::variant<ReadFromFile, ShowHelp>;

// Parses program arguments into a CommandLine object.
// Throws UnknownCommandLine if unable (i.e. arguments passed
// to the program are incorrect). Does not access argv[0].
CommandLine parseCommandLine(int argc, char* argv[]);
