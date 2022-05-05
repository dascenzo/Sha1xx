#include "Sha1.h"
#include "CommandLine.h"
#include <iostream>
#include <fstream>
#include <libgen.h>
#include <string>

static const char* gProgramName;

struct OperationDispatcher {
  void operator()(const ReadFromFile& fromFile) const;
  void operator()(const ShowHelp&) const;
};
struct FileError : std::runtime_error {
  using runtime_error::runtime_error;
};

int main(int argc, char* argv[]) {
  gProgramName = basename(argv[0]);
  try {
    const auto commandLine = parseCommandLine(argc, argv);
    std::visit(OperationDispatcher{}, commandLine);
    return 0;
  }
  catch (const UnknownCommandLine& e) {
    std::cerr << gProgramName << ": " << e.what() << std::endl;
    std::cerr << "help: " << gProgramName << " -h" << std::endl;
  }
  catch (const FileError& e) {
    std::cerr << gProgramName << ": error: " << e.what() << std::endl;
  }
  catch (const std::exception& e) {
    std::cerr << gProgramName << ": internal error: " << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << gProgramName << ": internal error" << std::endl;
  }
  return EXIT_FAILURE;
}
void OperationDispatcher::operator()(const ReadFromFile& fromFile) const {
  std::ifstream in{fromFile.fileName, std::ios::binary | std::ios::in};
  if (!in.is_open()) {
    throw FileError{std::string{"unable to open file: "} + fromFile.fileName};
  }
  Sha1 sha1{};
  if (in >> sha1) {
    std::cout << sha1.toString() << std::endl;
  }
  else {
    throw FileError{
        std::string{"encountered problem while reading file: "} + fromFile.fileName};
  }
}
void OperationDispatcher::operator()(const ShowHelp&) const {
  std::cout << gProgramName << ": this program prints the SHA-1 hash of the given file." << std::endl;
  std::cout << "usage: " << gProgramName << " [-h] [--] file" << std::endl;
}
