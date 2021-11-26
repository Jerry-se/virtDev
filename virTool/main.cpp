#include <iostream>
#include <string>
#include "virtDev.h"

void parseCommand(int argc, char *argv[]) {
  std::string op, param;
  if (argc < 2) {
    std::cout << "format: virTool operation [domain name]" << std::endl;
    return;
  }
  op = argv[1];
  if (argc > 2) param = argv[2];
  if (op == "ver" || op == "version")
    virTool::parseVersion();
  else if (op == "list")
    virTool::parseList();
  else if (op == "create") {
    if (param.empty()) {
      std::cout << "format: virTool operation [xml path]" << std::endl;
      return;
    }
    virTool::parseCreateDomain(param.c_str());
  }
  else if (op == "detail") {
    if (param.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virTool::parseDetailDomain(param.c_str());
  }
  else if (op == "suspend") {
    if (param.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virTool::parseSuspendDomain(param.c_str());
  }
  else if (op == "resume") {
    if (param.empty()) {
      std::cout << "format: virTool operation [domain name]" << std::endl;
      return;
    }
    virTool::parseResumeDomain(param.c_str());
  }
  else
    std::cout << "unknown operation" << std::endl;
}

int main(int argc, char *argv[]) {
  parseCommand(argc, argv);
  return 0;
}
