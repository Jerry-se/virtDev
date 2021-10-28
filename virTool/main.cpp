#include <iostream>
#include <string>
#include "virtDev.h"

void parseCommand(int argc, char *argv[]) {
  std::string op, domName;
  if (argc < 2) {
    std::cout << "format: virTool operation [domain name]" << std::endl;
    return;
  }
  op = argv[1];
  if (argc > 2) domName = argv[2];
  if (op == "ver" || op == "version")
    virtDev::parseVersion();
  else if (op == "list")
    virtDev::parseList();
  else if (op == "suspend") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virtDev::parseSuspendDomain(domName.c_str());
  }
  else if (op == "resume") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virtDev::parseResumeDomain(domName.c_str());
  }
  else
    std::cout << "unknown operation" << std::endl;
}

int main(int argc, char *argv[]) {
  parseCommand(argc, argv);
  return 0;
}
