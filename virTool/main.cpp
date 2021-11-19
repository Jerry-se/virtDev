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
    virTool::parseVersion();
  else if (op == "list")
    virTool::parseList();
  else if (op == "create") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [xml path]" << std::endl;
      return;
    }
    virTool::parseCreateDomain(domName.c_str());
  }
  else if (op == "suspend") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virTool::parseSuspendDomain(domName.c_str());
  }
  else if (op == "resume") {
    if (domName.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virTool::parseResumeDomain(domName.c_str());
  }
  else
    std::cout << "unknown operation" << std::endl;
}

int main(int argc, char *argv[]) {
  parseCommand(argc, argv);
  return 0;
}
